#include "EduToolPortal.hpp"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QBoxLayout>
#include <QFormLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>
#include <QDateTime>
#include <QCryptographicHash>
#include <QUuid>
#include <QUrlQuery>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

static QByteArray protectToken(const QByteArray &data, bool encrypt)
{
#ifdef _WIN32
	DATA_BLOB input{DWORD(data.size()), reinterpret_cast<BYTE *>(const_cast<char *>(data.constData()))}, output{};
	const bool ok = encrypt ? CryptProtectData(&input, L"EduTool session", nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &output)
		: CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &output);
	if (!ok) return {};
	QByteArray result(reinterpret_cast<const char *>(output.pbData), int(output.cbData));
	SecureZeroMemory(output.pbData, output.cbData); LocalFree(output.pbData); return result;
#else
	Q_UNUSED(data); Q_UNUSED(encrypt); return {};
#endif
}

EduToolPortal::EduToolPortal(QWidget *parent) : QWidget(parent)
{
	setObjectName("eduToolPortal"); network = new QNetworkAccessManager(this);
	auto *outer = new QVBoxLayout(this); auto *scroll = new QScrollArea(this); scroll->setWidgetResizable(true); outer->addWidget(scroll);
	auto *content = new QWidget(scroll); scroll->setWidget(content); auto *layout = new QVBoxLayout(content); layout->setContentsMargins(28, 24, 28, 24);
	auto *heading = new QLabel(QStringLiteral("홈페이지 업로드"), content); heading->setObjectName("eduToolFeatureHeading"); layout->addWidget(heading);
	account = new QLabel(QStringLiteral("로그인하지 않음"), content); account->setTextFormat(Qt::PlainText); layout->addWidget(account);
	auto *connection = new QHBoxLayout; layout->addLayout(connection);
	auto button = [this](QBoxLayout *row, const QString &text, auto callback) { auto *b = new QPushButton(text, this); row->addWidget(b); connect(b, &QPushButton::clicked, this, callback); return b; };
	button(connection, QStringLiteral("홈페이지 연결 설정"), [this]() { configure(); });
	button(connection, QStringLiteral("로그아웃"), [this]() { logout(); });
	auto *form = new QFormLayout; layout->addLayout(form);
	email = new QLineEdit(content); password = new QLineEdit(content); password->setEchoMode(QLineEdit::Password);
	form->addRow(QStringLiteral("이메일"), email); form->addRow(QStringLiteral("비밀번호"), password);
	auto *show = new QCheckBox(QStringLiteral("비밀번호 표시"), content); form->addRow(show);
	connect(show, &QCheckBox::toggled, this, [this](bool checked) { password->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password); });
	remember = new QCheckBox(QStringLiteral("로그인 상태 유지 (이 Windows 사용자에 암호화 저장)"), content); form->addRow(remember);
#ifndef _WIN32
	remember->setEnabled(false); remember->setToolTip(QStringLiteral("이 운영체제에서는 세션을 저장하지 않습니다."));
#endif
	auto *loginButtons = new QHBoxLayout; layout->addLayout(loginButtons);
	button(loginButtons, QStringLiteral("로그인"), [this]() { login(); });
	button(loginButtons, QStringLiteral("Google 로그인"), [this]() { oauth("google"); });
	button(loginButtons, QStringLiteral("Microsoft 로그인"), [this]() { oauth("microsoft"); });
	folders = new QComboBox(content); form->addRow(QStringLiteral("강좌 / 폴더"), folders);
	auto *folderButtons = new QHBoxLayout; layout->addLayout(folderButtons);
	button(folderButtons, QStringLiteral("목록 새로고침"), [this]() { loadFolders(); });
	button(folderButtons, QStringLiteral("새 폴더"), [this]() { newFolder(); });
	file = new QLineEdit(content); file->setReadOnly(true); title = new QLineEdit(content);
	form->addRow(QStringLiteral("동영상"), file); form->addRow(QStringLiteral("영상 제목"), title);
	button(folderButtons, QStringLiteral("영상 선택"), [this]() { const auto path = QFileDialog::getOpenFileName(this, QStringLiteral("업로드할 영상"), {}, QStringLiteral("영상 (*.mp4 *.mkv *.mov *.avi *.webm)")); if (!path.isEmpty()) offerFile(path); });
	automatic = new QCheckBox(QStringLiteral("녹화 종료 후 자동 업로드"), content);
	notify = new QCheckBox(QStringLiteral("업로드 완료 알림"), content);
	removeOriginal = new QCheckBox(QStringLiteral("서버가 파일 전체 검증을 완료한 뒤 원본 삭제"), content);
	for (auto *option : {automatic, notify, removeOriginal}) { layout->addWidget(option); connect(option, &QCheckBox::toggled, this, [this](bool) { persistOptions(); }); }
	progress = new QProgressBar(content); progress->setRange(0, 100); layout->addWidget(progress);
	auto *actions = new QHBoxLayout; layout->addLayout(actions);
	upload = button(actions, QStringLiteral("업로드 / 재시도"), [this]() { uploadFile(); });
	button(actions, QStringLiteral("대기 녹화 업로드"), [this]() { nextRecording(); });
	auto *cancelButton = button(actions, QStringLiteral("작업 취소"), [this]() { cancel(); }); cancelButton->setObjectName("eduToolCancelUpload");
	status = new QLabel(content); status->setWordWrap(true); status->setTextFormat(Qt::PlainText); layout->addWidget(status); layout->addStretch();
	QSettings settings("EduTool", "Studio"); base = QUrl(settings.value("portal/base").toString());
	for (auto *option : {automatic, notify, removeOriginal}) option->blockSignals(true);
	automatic->setChecked(settings.value("portal/automatic", false).toBool()); notify->setChecked(settings.value("portal/notify", true).toBool()); removeOriginal->setChecked(settings.value("portal/removeOriginal", false).toBool());
	for (auto *option : {automatic, notify, removeOriginal}) option->blockSignals(false);
	if (settings.value("portal/sessionBase").toString() == base.toString()) token = protectToken(settings.value("portal/session").toByteArray(), false);
	remember->setChecked(!token.isEmpty());
	queuedRecordings = settings.value("portal/queued").toJsonArray();
	connect(remember, &QCheckBox::toggled, this, [this](bool checked) {
		QSettings s("EduTool", "Studio"); if (!checked) s.remove("portal/session"); else if (!token.isEmpty()) { s.setValue("portal/session", protectToken(token, true)); s.setValue("portal/sessionBase", base.toString()); }
	});
	connect(folders, &QComboBox::currentIndexChanged, this, [this](int) { persistOptions(); });
	oauthPoll = new QTimer(this); oauthPoll->setInterval(3000);
	connect(oauthPoll, &QTimer::timeout, this, [this]() {
		if (active) return;
		if (QDateTime::currentSecsSinceEpoch() >= oauthSession["expires_at"].toDouble()) { cancel(); message(QStringLiteral("로그인 시간이 만료되었습니다.")); return; }
		request("POST", "/v1/auth/browser/poll", {{"poll_token", oauthSession["poll_token"]}}, [this](const QJsonObject &json) {
			if (json["status"] == "pending") { setBusy(true); return; }
			oauthPoll->stop(); session(json);
		});
	});
	const auto job = settings.value("portal/job").toJsonObject();
	if (!job.isEmpty() && job["state"] != "complete") {
		currentFile = job["path"].toString(); file->setText(currentFile); title->setText(job["title"].toString());
		if (job["base"].toString() == base.toString()) settings.setValue("portal/folder", job["folder"].toString());
		message(QStringLiteral("이전 업로드가 완료되지 않았습니다. 로그인과 대상을 확인한 뒤 재시도하세요. 원본은 유지됩니다."));
	} else message(validBase(base) ? QStringLiteral("홈페이지에 로그인하세요.") : QStringLiteral("연결된 홈페이지가 없습니다. 홈페이지 연결 설정에서 서버 주소를 입력하세요."));
	if (!token.isEmpty() && validBase(base)) QTimer::singleShot(0, this, [this]() { request("GET", "/v1/me", {}, [this](const QJsonObject &json) { identity(json); loadFolders(); }); });
}
EduToolPortal::~EduToolPortal() { cancel(); }
bool EduToolPortal::validBase(const QUrl &url) const
{
	return url.isValid() && !url.host().isEmpty() && url.userInfo().isEmpty() && !url.hasQuery() && !url.hasFragment() &&
		(url.scheme() == "https" || (url.scheme() == "http" && (url.host() == "localhost" || url.host() == "127.0.0.1")));
}
void EduToolPortal::message(const QString &text) { status->setText(text); }
void EduToolPortal::setBusy(bool value)
{
	busy = value;
	for (auto *button : findChildren<QPushButton *>()) if (button->objectName() != "eduToolCancelUpload") button->setEnabled(!value);
	for (QWidget *widget : std::initializer_list<QWidget *>{email, password, file, title, folders, automatic, notify, removeOriginal, remember}) widget->setEnabled(!value);
}
void EduToolPortal::configure()
{
	if (busy) return;
	bool ok;
	const auto value = QInputDialog::getText(this, QStringLiteral("홈페이지 연결"), QStringLiteral("EduTool 서버 어댑터의 HTTPS 주소"), QLineEdit::Normal, base.toString(), &ok);
	if (!ok) return; QUrl candidate(value.trimmed());
	if (!validBase(candidate)) { message(QStringLiteral("HTTPS 주소를 입력하세요. 로컬 개발 서버만 HTTP를 사용할 수 있습니다.")); return; }
	logout(); base = candidate.adjusted(QUrl::StripTrailingSlash); QSettings("EduTool", "Studio").setValue("portal/base", base.toString()); message(QStringLiteral("홈페이지 주소를 저장했습니다. 로그인하세요."));
}
void EduToolPortal::request(const QString &method, const QString &path, const QJsonObject &body, std::function<void(const QJsonObject &)> done)
{
	if (active) return;
	if (!validBase(base)) { message(QStringLiteral("홈페이지 연결 설정이 필요합니다.")); return; }
	QNetworkRequest request(QUrl(base.toString(QUrl::FullyEncoded) + path));
	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); request.setTransferTimeout(30000);
	if (!token.isEmpty()) request.setRawHeader("Authorization", "Bearer " + token);
	const int epoch = generation;
	QNetworkReply *reply = method == "GET" ? network->get(request) : network->sendCustomRequest(request, method.toLatin1(), QJsonDocument(body).toJson(QJsonDocument::Compact));
	active = reply; setBusy(true);
	connect(reply, &QNetworkReply::readyRead, reply, [reply]() { if (reply->bytesAvailable() > 2 * 1024 * 1024) reply->abort(); });
	connect(reply, &QNetworkReply::finished, this, [this, reply, epoch, done]() {
		reply->deleteLater(); if (epoch != generation) return;
		active = nullptr; setBusy(false);
		const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (reply->error() != QNetworkReply::NoError || code < 200 || code >= 300) { handleFailure(reply); return; }
		QJsonParseError error; const auto document = QJsonDocument::fromJson(reply->readAll(), &error);
		if (error.error != QJsonParseError::NoError || !document.isObject()) { message(QStringLiteral("서버 응답 형식이 올바르지 않습니다.")); return; }
		done(document.object());
	});
}
void EduToolPortal::handleFailure(QNetworkReply *reply)
{
	const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (code == 401) { token.clear(); QSettings("EduTool", "Studio").remove("portal/session"); account->setText(QStringLiteral("로그인이 만료되었습니다.")); emit accountChanged(QStringLiteral("로그인")); }
	oauthPoll->stop(); persistJob("failed");
	if (code == 409) { message(QStringLiteral("같은 제목의 다른 영상이 있습니다. 제목을 바꾸고 다시 업로드하세요. 원본은 유지됩니다.")); return; }
	message(code == 401 || code == 403 ? QStringLiteral("로그인 또는 대상 폴더 권한을 확인하세요. 원본은 유지됩니다.")
		: QStringLiteral("서버 요청 실패 (HTTP %1). 연결을 확인하고 재시도하세요. 원본은 유지됩니다.").arg(code));
}
void EduToolPortal::login()
{
	if (busy) return;
	if (!email->text().contains('@') || password->text().isEmpty()) { message(QStringLiteral("이메일과 비밀번호를 입력하세요.")); return; }
	const QJsonObject body{{"email", email->text().trimmed()}, {"password", password->text()}};
	request("POST", "/v1/auth/login", body, [this](const QJsonObject &json) { session(json); }); password->clear();
}
void EduToolPortal::session(const QJsonObject &json)
{
	const QByteArray received = json["access_token"].toString().toUtf8();
	if (received.isEmpty() || received.contains('\r') || received.contains('\n')) { message(QStringLiteral("로그인 토큰을 확인할 수 없습니다.")); return; }
	token = received;
	if (remember->isChecked()) { QSettings s("EduTool", "Studio"); s.setValue("portal/session", protectToken(token, true)); s.setValue("portal/sessionBase", base.toString()); }
	identity(json); message(QStringLiteral("로그인했습니다.")); loadFolders();
}
void EduToolPortal::identity(const QJsonObject &json)
{
	const auto name = json["name"].toString(QStringLiteral("로그인됨"));
	const auto address = json["email"].toString();
	account->setText(QStringLiteral("%1\n%2\n연결 홈페이지: %3").arg(name, address, base.toString()));
	if (!address.isEmpty()) email->setText(address);
	emit accountChanged(name);
}
void EduToolPortal::logout()
{
	cancel();
	if (!token.isEmpty() && validBase(base)) {
		QNetworkRequest revoke(QUrl(base.toString(QUrl::FullyEncoded) + "/v1/auth/logout"));
		revoke.setRawHeader("Authorization", "Bearer " + token); revoke.setTransferTimeout(10000);
		revoke.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
		auto *reply = network->post(revoke, QByteArray()); connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
	}
	token.clear(); folders->clear(); QSettings("EduTool", "Studio").remove("portal/session"); account->setText(QStringLiteral("로그인하지 않음")); emit accountChanged(QStringLiteral("로그인"));
}
void EduToolPortal::oauth(const QString &provider)
{
	if (busy) return;
	request("POST", "/v1/auth/browser", {{"provider", provider}}, [this](const QJsonObject &json) {
		const QUrl url(json["authorization_url"].toString());
		if (url.scheme() != "https" || url.host().isEmpty() || !url.userInfo().isEmpty() || json["poll_token"].toString().isEmpty() || json["expires_at"].toDouble() <= QDateTime::currentSecsSinceEpoch()) { message(QStringLiteral("서버의 외부 로그인 설정을 확인하세요.")); return; }
		if (!QDesktopServices::openUrl(url)) { message(QStringLiteral("로그인 브라우저를 열 수 없습니다.")); return; }
		oauthSession = json; oauthPoll->start(); setBusy(true); message(QStringLiteral("브라우저에서 로그인하세요. 이 창에서 작업 취소로 중단할 수 있습니다."));
	});
}
void EduToolPortal::loadFolders()
{
	if (token.isEmpty() || busy) { message(QStringLiteral("먼저 로그인하세요.")); return; }
	request("GET", "/v1/folders", {}, [this](const QJsonObject &json) {
		const auto previous = QSettings("EduTool", "Studio").value("portal/folder").toString(); folders->blockSignals(true); folders->clear();
		for (const auto &entry : json["folders"].toArray()) { const auto folder = entry.toObject(); if (!folder["id"].toString().isEmpty()) folders->addItem(folder["name"].toString(), folder["id"].toString()); }
		const int index = folders->findData(previous); if (index >= 0) folders->setCurrentIndex(index); folders->blockSignals(false); persistOptions();
		message(folders->count() ? QStringLiteral("대상 폴더와 제목을 확인한 뒤 업로드하세요.") : QStringLiteral("사용 가능한 폴더가 없습니다."));
	});
}
void EduToolPortal::newFolder()
{
	if (busy || token.isEmpty()) return; bool ok;
	const auto name = QInputDialog::getText(this, QStringLiteral("새 폴더"), QStringLiteral("폴더 이름"), QLineEdit::Normal, {}, &ok).trimmed();
	if (!ok || name.isEmpty()) return;
	request("POST", "/v1/folders", {{"name", name}}, [this](const QJsonObject &json) { QSettings("EduTool", "Studio").setValue("portal/folder", json["id"].toString()); loadFolders(); });
}
void EduToolPortal::offerFile(const QString &path)
{
	if (busy) { message(QStringLiteral("현재 업로드 작업을 먼저 완료하거나 취소하세요.")); return; }
	currentFile = QFileInfo(path).absoluteFilePath(); file->setText(currentFile); title->setText(QFileInfo(path).completeBaseName()); message(QStringLiteral("파일을 선택했습니다. 업로드 버튼으로 전송하세요."));
}
void EduToolPortal::recordingFinished(const QString &path)
{
	if (!automatic->isChecked()) return;
	queuedRecordings.append(QJsonObject{{"path", path}, {"folder", folders->currentData().toString()}, {"base", base.toString()}});
	QSettings("EduTool", "Studio").setValue("portal/queued", queuedRecordings);
	if (busy) { message(QStringLiteral("업로드 대기에 녹화를 추가했습니다. 원본은 로컬에 보존됩니다.")); return; }
	nextRecording();
}
void EduToolPortal::nextRecording()
{
	if (busy || queuedRecordings.isEmpty()) return;
	if (token.isEmpty()) { message(QStringLiteral("대기 녹화를 업로드하려면 로그인하세요.")); return; }
	const auto job = queuedRecordings.first().toObject();
	const int index = folders->findData(job["folder"].toString());
	if (job["base"].toString() != base.toString() || index < 0) { message(QStringLiteral("대기 녹화의 원래 홈페이지·대상 폴더에 연결하세요. 파일은 로컬에 보존됩니다.")); return; }
	folders->setCurrentIndex(index); offerFile(job["path"].toString());
	queuedRecordings.removeFirst(); QSettings("EduTool", "Studio").setValue("portal/queued", queuedRecordings);
	persistJob("queued"); uploadFile();
}
void EduToolPortal::persistOptions()
{
	QSettings s("EduTool", "Studio"); s.setValue("portal/automatic", automatic->isChecked()); s.setValue("portal/notify", notify->isChecked()); s.setValue("portal/removeOriginal", removeOriginal->isChecked()); s.setValue("portal/folder", folders->currentData());
}
void EduToolPortal::persistJob(const QString &state)
{
	if (currentFile.isEmpty()) return;
	QSettings("EduTool", "Studio").setValue("portal/job", QJsonObject{{"state", state}, {"path", currentFile}, {"title", title->text()}, {"id", uploadId}, {"key", uploadKey}, {"folder", folders->currentData().toString()}, {"base", base.toString()}});
}
void EduToolPortal::uploadFile()
{
	if (busy) return;
	if (token.isEmpty() || folders->currentData().toString().isEmpty() || !validBase(base)) { message(QStringLiteral("로그인과 업로드 대상 폴더를 먼저 선택하세요.")); return; }
	if (title->text().trimmed().isEmpty() || title->text().size() > 200) { message(QStringLiteral("영상 제목은 1~200자로 입력하세요.")); return; }
	const QFileInfo info(currentFile);
	if (!info.isFile() || info.size() <= 0 || !QStringList{"mp4", "mkv", "mov", "avi", "webm"}.contains(info.suffix().toLower())) { message(QStringLiteral("읽을 수 있는 영상 파일을 선택하세요.")); return; }
	expectedSize = info.size(); jobRemoveOriginal = removeOriginal->isChecked();
	const int epoch = generation; const QString path = currentFile; const qint64 size = expectedSize;
	setBusy(true); message(QStringLiteral("파일 검증 정보를 계산하고 있습니다."));
	auto *watcher = new QFutureWatcher<QByteArray>(this);
	connect(watcher, &QFutureWatcher<QByteArray>::finished, this, [this, watcher, epoch]() {
		const auto hash = watcher->result(); watcher->deleteLater(); if (epoch != generation) return;
		setBusy(false);
		if (hash.isEmpty()) { message(QStringLiteral("원본을 읽지 못했거나 계산 중 파일 크기가 변경되었습니다.")); return; }
		expectedHash = QString::fromLatin1(hash.toHex()); sendUpload();
	});
	watcher->setFuture(QtConcurrent::run([path, size]() -> QByteArray {
		QFile input(path); QCryptographicHash hash(QCryptographicHash::Sha256);
		if (!input.open(QIODevice::ReadOnly) || input.size() != size || !hash.addData(&input) || input.size() != size) return {};
		return hash.result();
	}));
}
void EduToolPortal::sendUpload()
{
	const QFileInfo info(currentFile);
	if (info.size() != expectedSize) { message(QStringLiteral("계산 후 파일 크기가 변경되었습니다. 다시 시도하세요.")); return; }
	uploadKey = QString::fromLatin1(QCryptographicHash::hash((base.toString() + folders->currentData().toString() + title->text().trimmed() + expectedHash).toUtf8(), QCryptographicHash::Sha256).toHex());
	auto *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart metadata; metadata.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=metadata"); metadata.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	metadata.setBody(QJsonDocument(QJsonObject{{"folder_id", folders->currentData().toString()}, {"title", title->text().trimmed()}, {"sha256", expectedHash}, {"size", expectedSize}, {"collision", "reject"}}).toJson(QJsonDocument::Compact)); multipart->append(metadata);
	auto *body = new QFile(currentFile, multipart); if (!body->open(QIODevice::ReadOnly)) { delete multipart; message(QStringLiteral("원본 파일을 열 수 없습니다.")); return; }
	QHttpPart binary; binary.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=file; filename=video." + info.suffix().toLatin1()); binary.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream"); binary.setBodyDevice(body); multipart->append(binary);
	QNetworkRequest request(QUrl(base.toString(QUrl::FullyEncoded) + "/v1/uploads")); request.setRawHeader("Authorization", "Bearer " + token); request.setRawHeader("Idempotency-Key", uploadKey.toLatin1()); request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy); request.setTransferTimeout(60000);
	auto *reply = network->post(request, multipart); multipart->setParent(reply); active = reply; const int epoch = generation; setBusy(true); persistJob("uploading");
	connect(reply, &QNetworkReply::uploadProgress, this, [this](qint64 sent, qint64 size) { if (size > 0) progress->setValue(int(95.0 * sent / size)); });
	connect(reply, &QNetworkReply::readyRead, reply, [reply]() { if (reply->bytesAvailable() > 2 * 1024 * 1024) reply->abort(); });
	connect(reply, &QNetworkReply::finished, this, [this, reply, epoch]() {
		reply->deleteLater(); if (epoch != generation) return; active = nullptr; setBusy(false);
		const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (reply->error() != QNetworkReply::NoError || code < 200 || code >= 300) { handleFailure(reply); return; }
		uploadId = QJsonDocument::fromJson(reply->readAll()).object()["id"].toString();
		if (uploadId.isEmpty()) { message(QStringLiteral("서버 업로드 ID를 확인할 수 없습니다. 원본은 유지합니다.")); return; }
		persistJob("verifying"); verifyUpload();
	});
}
void EduToolPortal::verifyUpload()
{
	request("GET", "/v1/uploads/" + QString::fromLatin1(QUrl::toPercentEncoding(uploadId)), {}, [this](const QJsonObject &json) {
		if (json["status"] != "complete" || !json["verified"].toBool() || json["sha256"].toString().compare(expectedHash, Qt::CaseInsensitive) || qint64(json["size"].toDouble()) != expectedSize || json["title"].toString() != title->text().trimmed() || json["folder_id"].toString() != folders->currentData().toString()) {
			persistJob("unverified"); message(QStringLiteral("서버의 파일 검증이 완료되지 않았습니다. 원본을 유지합니다. 재시도로 상태를 확인하세요.")); return;
		}
			auto complete = [this](const QString &suffix) {
			const QString result = QStringLiteral("업로드와 서버 파일 검증이 완료되었습니다.") + suffix;
			progress->setValue(100); persistJob("complete"); message(result);
			if (notify->isChecked()) QMessageBox::information(this, QStringLiteral("업로드 완료"), result);
			QTimer::singleShot(0, this, [this]() { nextRecording(); });
		};
		if (!jobRemoveOriginal) { complete({}); return; }
		const int epoch = generation; const QString path = currentFile; const auto size = expectedSize; const auto expected = expectedHash;
		setBusy(true); message(QStringLiteral("원본 변경 여부를 확인하고 있습니다."));
		auto *watcher = new QFutureWatcher<bool>(this);
		connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, epoch, path, complete]() {
			const bool unchanged = watcher->result(); watcher->deleteLater(); if (epoch != generation) return; setBusy(false);
			complete(unchanged && QFile::remove(path) ? QStringLiteral(" 원본을 삭제했습니다.") : QStringLiteral(" 원본이 변경되었거나 삭제할 수 없어 보존했습니다."));
		});
		watcher->setFuture(QtConcurrent::run([path, size, expected]() {
			QFile original(path); QCryptographicHash hash(QCryptographicHash::Sha256);
			return original.open(QIODevice::ReadOnly) && original.size() == size && hash.addData(&original) && original.size() == size && QString::fromLatin1(hash.result().toHex()) == expected;
		}));
	});
}
void EduToolPortal::cancel()
{
	++generation; if (oauthPoll) oauthPoll->stop(); if (active) { active->abort(); active = nullptr; }
	if (busy) persistJob("cancelled"); setBusy(false); message(QStringLiteral("작업을 중단했습니다. 원본은 유지됩니다."));
}
bool EduToolPortal::confirmClose()
{
	if (!busy) return true;
	if (QMessageBox::question(this, QStringLiteral("업로드 중"), QStringLiteral("진행 중인 작업을 중단하고 종료할까요? 원본은 유지되며 다음 실행에서 재시도할 수 있습니다."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return false;
	cancel(); return true;
}
