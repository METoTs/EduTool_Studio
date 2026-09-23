#include "EduToolEditor.hpp"
#include "EduToolFileBrowser.hpp"
#include <QDialog>
#include <QSizePolicy>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVideoSink>
#include <QVideoFrame>
#include <QShortcut>
#include <QImage>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QTemporaryDir>
#include <QTimer>
#include <QDir>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <algorithm>
#include <cmath>
#include <functional>

static QString stamp(qint64 ms)
{
	return QStringLiteral("%1:%2:%3.%4").arg(ms / 3600000, 2, 10, QChar('0'))
		.arg(ms / 60000 % 60, 2, 10, QChar('0')).arg(ms / 1000 % 60, 2, 10, QChar('0'))
		.arg(ms % 1000, 3, 10, QChar('0'));
}

class EduToolStillFrame : public QWidget {
public:
	using QWidget::QWidget;
	QImage frame;
protected:
	void paintEvent(QPaintEvent *) override {
		QPainter painter(this); painter.fillRect(rect(), Qt::black);
		if (frame.isNull()) return;
		const auto size = frame.size().scaled(this->size(), Qt::KeepAspectRatio);
		painter.drawImage(QRect(QPoint((width() - size.width()) / 2, (height() - size.height()) / 2), size), frame);
	}
};

class EduToolTimeline : public QWidget {
public:
	QVector<EduToolClip> clips;
	qint64 length = 0, position = 0, from = 0, to = 0, anchor = 0;
	std::function<void(qint64)> scrub;
	std::function<void(qint64, qint64)> selection;
	bool dragging = false;
	QHash<QString, QImage> thumbnails;
	explicit EduToolTimeline(QWidget *parent) : QWidget(parent) { setMinimumHeight(112); }
	qint64 at(qreal x) const { return qBound<qint64>(0, qint64(x / std::max(1, width()) * length), length); }
protected:
	void paintEvent(QPaintEvent *) override
	{
		QPainter p(this);
		p.fillRect(rect(), QColor("#06192d"));
		if (!length) { p.setPen(Qt::white); p.drawText(rect(), Qt::AlignCenter, QStringLiteral("영상을 불러오세요")); return; }
		const double scale = double(width()) / length;
		qint64 cursor = 0;
		for (int i = 0; i < clips.size(); ++i) {
			const auto &clip = clips[i];
			QRectF box(cursor * scale, 30, (clip.out - clip.in) * scale, 60);
			p.fillRect(box.adjusted(1, 1, -1, -1), i % 2 ? QColor("#195a89") : QColor("#12456b"));
			if (thumbnails.contains(clip.path)) {
				const QRectF thumbnail(box.left() + 2, box.top() + 2, std::min(90.0, box.width()), 56);
				p.drawImage(thumbnail, thumbnails.value(clip.path));
			}
			p.setPen(Qt::white);
			p.drawText(box.adjusted(8, 2, -4, -2), Qt::AlignVCenter, clip.name);
			cursor += clip.out - clip.in;
		}
		const qint64 step = std::max<qint64>(1000, qint64(std::ceil(115.0 / scale / 1000.0)) * 1000);
		const QRect visible = visibleRegion().boundingRect();
		for (qint64 t = (at(visible.left()) / step) * step; t <= std::min(length, at(visible.right()) + step); t += step) {
			p.setPen(QColor("#bad8ed"));
			p.drawText(QPointF(t * scale + 3, 19), stamp(t));
			p.drawLine(QPointF(t * scale, 23), QPointF(t * scale, 30));
		}
		p.fillRect(QRectF(from * scale, 30, (to - from) * scale, 60), QColor(38, 153, 255, 80));
		p.setPen(QPen(QColor("#ff5c69"), 2));
		p.drawLine(QPointF(position * scale, 0), QPointF(position * scale, height()));
	}
	void mousePressEvent(QMouseEvent *e) override { anchor = at(e->position().x()); dragging = true; if (scrub) scrub(anchor); }
	void mouseMoveEvent(QMouseEvent *e) override { if (dragging && selection) selection(std::min(anchor, at(e->position().x())), std::max(anchor, at(e->position().x()))); }
	void mouseReleaseEvent(QMouseEvent *) override { dragging = false; }
};

QJsonObject EduToolClip::json() const
{
	return {{"path", path}, {"name", name}, {"in", in}, {"out", out}, {"duration", duration},
		{"width", width}, {"height", height}, {"fps", fps}, {"audio", audio}};
}
EduToolClip EduToolClip::fromJson(const QJsonObject &j)
{
	return {j["path"].toString(), j["name"].toString(), qint64(j["in"].toDouble()), qint64(j["out"].toDouble()),
		qint64(j["duration"].toDouble()), j["width"].toInt(), j["height"].toInt(), j["fps"].toDouble(), j["audio"].toBool()};
}

EduToolEditor::EduToolEditor(QWidget *parent) : QWidget(parent)
{
	setObjectName(QStringLiteral("eduToolEditor"));
	setAcceptDrops(true);
	auto *layout = new QVBoxLayout(this);
	layout->setContentsMargins(18, 18, 18, 18);
	auto *tools = new QHBoxLayout;
	layout->addLayout(tools);
	auto button = [this](QBoxLayout *row, const QString &text, auto callback) {
		auto *b = new QPushButton(text, this); row->addWidget(b); connect(b, &QPushButton::clicked, this, callback); return b;
	};
	button(tools, QStringLiteral("새 영상 열기"), [this]() { importFiles(EduToolFileBrowser::openFiles(this, QStringLiteral("영상 열기")), true); });
	button(tools, QStringLiteral("영상 추가"), [this]() { importFiles(EduToolFileBrowser::openFiles(this, QStringLiteral("영상 추가")), false); });
	insertMode = new QComboBox(this);
	insertMode->addItems({QStringLiteral("맨 뒤에 추가"), QStringLiteral("맨 앞에 추가"), QStringLiteral("현재 재생 위치에 추가")});
	tools->addWidget(insertMode);
	recent = new QComboBox(this); recent->addItem(QStringLiteral("최근 영상"));
	for (const auto &path : QSettings("EduTool", "Studio").value("editor/recent").toStringList()) recent->addItem(path, path);
	recent->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon); recent->setMinimumContentsLength(10);
	tools->addWidget(recent, 1);
	connect(recent, &QComboBox::activated, this, [this](int i) {
		if (i <= 0) return;
		QString path = recent->itemData(i).toString();
		if (!QFileInfo(path).isFile()) {
			QMessageBox message(QMessageBox::Warning, QStringLiteral("최근 영상 없음"), QStringLiteral("파일이 이동되었거나 삭제되었습니다.\n%1").arg(path), QMessageBox::Cancel, this);
			auto *locate = message.addButton(QStringLiteral("위치 찾기"), QMessageBox::ActionRole);
			auto *remove = message.addButton(QStringLiteral("목록에서 제거"), QMessageBox::ActionRole);
			message.exec();
			if (message.clickedButton() == remove) {
				QSettings settings("EduTool", "Studio"); auto paths = settings.value("editor/recent").toStringList(); paths.removeAll(path); settings.setValue("editor/recent", paths); recent->removeItem(i); return;
			}
			if (message.clickedButton() != locate) return;
			const auto paths = EduToolFileBrowser::openFiles(this, QStringLiteral("이동한 영상 찾기"));
			if (paths.isEmpty()) return;
			path = paths.first();
		}
		importFiles({path}, true);
	});
	auto *splitter = new QSplitter(this); previewSplitter = splitter;
	browser = new EduToolFileBrowser(splitter); browser->setMinimumWidth(220); browser->setMaximumWidth(400); browser->hide();
	button(tools, QStringLiteral("파일 탐색기"), [this]() {
		browser->setVisible(!browser->isVisible());
		if (browser->isVisible()) previewSplitter->setSizes({300, 650, 250});
	});
	button(tools, QStringLiteral("선택 파일 추가"), [this]() { importFiles(browser->selectedFiles(), clips.isEmpty()); });
	connect(browser, &EduToolFileBrowser::filesActivated, this, [this](const QStringList &paths) { importFiles(paths, clips.isEmpty()); });
	layout->addWidget(splitter, 1);
	preview = new QStackedWidget(splitter); preview->setMinimumSize(240, 135);
	preview->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	video = new QVideoWidget(preview); preview->addWidget(video);
	video->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	stillFrame = new EduToolStillFrame(preview); preview->addWidget(stillFrame);
	list = new QListWidget(splitter); list->setMinimumWidth(200); list->setWordWrap(true); list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	splitter->setStretchFactor(0, 0); splitter->setStretchFactor(1, 4); splitter->setStretchFactor(2, 1);
	QTimer::singleShot(0, this, [splitter]() { splitter->setSizes({0, 900, 250}); });
	player = new QMediaPlayer(this); audio = new QAudioOutput(this); audio->setVolume(0.7f);
	player->setAudioOutput(audio); player->setVideoOutput(video);
	connect(video->videoSink(), &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
		if (activeClip >= 0 && activeClip < clips.size() && !timeline->thumbnails.contains(clips[activeClip].path) && frame.isValid()) {
			const auto thumbnail = frame.toImage().scaled(160, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			if (!thumbnail.isNull()) timeline->thumbnails.insert(clips[activeClip].path, thumbnail);
			timeline->update();
		}
	});
	auto *escape = new QShortcut(QKeySequence(Qt::Key_Escape), preview);
	connect(escape, &QShortcut::activated, preview, [this]() { if (fullscreen) fullscreen->reject(); });
	auto *playback = new QHBoxLayout; layout->addLayout(playback);
	button(playback, QStringLiteral("재생 / 일시정지"), [this]() {
		if (pendingPlay) { pendingPlay = false; player->pause(); }
		else { selectionPlayback = false; seek(position >= total() ? 0 : position, true); }
	});
	button(playback, QStringLiteral("전체화면"), [this]() { toggleFullscreen(); });
	auto *volume = new QSlider(Qt::Horizontal, this); volume->setRange(0, 100); volume->setValue(70); volume->setMaximumWidth(130);
	volume->setAccessibleName(QStringLiteral("편집 미리보기 볼륨")); playback->addWidget(volume);
	connect(volume, &QSlider::valueChanged, this, [this](int v) { audio->setVolume(v / 100.0f); });
	clock = new QLabel(this); playback->addWidget(clock, 1);
	auto *scroll = new QScrollArea(this); scroll->setWidgetResizable(false); scroll->setFixedHeight(140);
	timeline = new EduToolTimeline(scroll); timeline->resize(800, 112); scroll->setWidget(timeline); layout->addWidget(scroll);
	timeline->scrub = [this](qint64 p) { selectionPlayback = false; seek(p); };
	timeline->selection = [this](qint64 a, qint64 b) { start->setValue(a / 1000.0); end->setValue(b / 1000.0); };
	auto *selectionRow = new QHBoxLayout; layout->addLayout(selectionRow);
	auto *edits = selectionRow;
	start = new QDoubleSpinBox(this); end = new QDoubleSpinBox(this);
	for (auto *spin : {start, end}) { spin->setDecimals(3); spin->setSuffix(QStringLiteral(" 초")); edits->addWidget(spin); }
	start->setAccessibleName(QStringLiteral("선택 시작")); end->setAccessibleName(QStringLiteral("선택 끝"));
	for (auto *spin : {start, end}) connect(spin, &QDoubleSpinBox::valueChanged, this, [this](double) {
		timeline->from = qint64(start->value() * 1000); timeline->to = qint64(end->value() * 1000); timeline->update();
	});
	button(edits, QStringLiteral("선택 재생"), [this]() {
		if (end->value() <= start->value()) { showError(QStringLiteral("끝 시간을 시작 시간보다 뒤로 설정하세요.")); return; }
		selectionPlayback = true; seek(qint64(start->value() * 1000), true);
	});
	button(edits, QStringLiteral("선택 해제"), [this]() { selectionPlayback = false; start->setValue(0); end->setValue(0); });
	auto *editRow = new QHBoxLayout; layout->addLayout(editRow); edits = editRow;
	button(edits, QStringLiteral("구간 삭제"), [this]() { removeSelection(); });
	button(edits, QStringLiteral("나누기"), [this]() { split(); });
	button(edits, QStringLiteral("실행 취소"), [this]() { if (!undo.isEmpty() && !exporting && !importing) { redo.push_back(snapshot()); restore(undo.takeLast()); } });
	button(edits, QStringLiteral("다시 실행"), [this]() { if (!redo.isEmpty() && !exporting && !importing) { undo.push_back(snapshot()); restore(redo.takeLast()); } });
	auto *bottom = new QHBoxLayout; layout->addLayout(bottom);
	zoom = new QSlider(Qt::Horizontal, this); zoom->setRange(1, 30); zoom->setValue(1); zoom->setMaximumWidth(120);
	zoom->setAccessibleName(QStringLiteral("타임라인 확대")); bottom->addWidget(zoom);
	auto resizeTimeline = [this, scroll]() { timeline->resize(std::max(400, scroll->viewport()->width()) * zoom->value(), 112); timeline->update(); };
	connect(zoom, &QSlider::valueChanged, this, [resizeTimeline](int) { resizeTimeline(); });
	button(bottom, QStringLiteral("전체 보기"), [this, resizeTimeline]() { zoom->setValue(1); resizeTimeline(); });
	button(bottom, QStringLiteral("작업 열기"), [this]() { loadProject(); });
	button(bottom, QStringLiteral("작업 저장"), [this]() { saveProject(); });
	auto *exportRow = new QHBoxLayout; layout->addLayout(exportRow); bottom = exportRow;
	button(bottom, QStringLiteral("조각 저장"), [this]() { exportVideo(true); });
	button(bottom, QStringLiteral("영상 내보내기"), [this]() { exportVideo(false); });
	button(bottom, QStringLiteral("완료 영상 업로드"), [this]() {
		if (exporting || exportPath.isEmpty() || !QFileInfo(exportPath).isFile()) { showError(QStringLiteral("먼저 영상 내보내기를 완료하세요.")); return; }
		emit uploadRequested(exportPath);
	});
	auto *cancel = button(bottom, QStringLiteral("작업 취소"), [this]() {
		if (importing) { importFailed(QStringLiteral("가져오기를 취소했습니다.")); }
		if (exporting && encoder) { abortExport = true; encoder->kill(); }
	});
	cancel->setObjectName(QStringLiteral("eduToolCancelMediaJob"));
	status = new QLabel(QStringLiteral("영상 파일을 열거나 이 화면에 끌어놓으세요."), this); status->setWordWrap(true); status->setTextFormat(Qt::PlainText); layout->addWidget(status);
	connect(list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
		if (exporting || importing) return;
		const int index = list->row(item); bool ok;
		const auto name = QInputDialog::getText(this, QStringLiteral("조각 이름"), QStringLiteral("이름"), QLineEdit::Normal, clips[index].name, &ok);
		if (ok && !name.trimmed().isEmpty()) { checkpoint(); clips[index].name = name.trimmed(); refresh(); }
	});
	list->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(list, &QListWidget::customContextMenuRequested, this, [this](const QPoint &) {
		if (exporting || importing || list->currentRow() < 0) return;
		const int index = list->currentRow();
		const auto choice = QInputDialog::getItem(this, QStringLiteral("조각 순서"), QStringLiteral("이동"), {QStringLiteral("위로"), QStringLiteral("아래로")}, 0, false);
		const int target = choice == QStringLiteral("위로") ? index - 1 : choice == QStringLiteral("아래로") ? index + 1 : index;
		if (target >= 0 && target < clips.size() && target != index) { checkpoint(); clips.swapItemsAt(index, target); refresh(); list->setCurrentRow(target); }
	});
	connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus state) {
		if (awaitingMedia && (state == QMediaPlayer::LoadedMedia || state == QMediaPlayer::BufferedMedia)) {
			awaitingMedia = false;
			applyPendingSeek();
		}
		else if (state == QMediaPlayer::EndOfMedia && pendingPlay && !awaitingMedia && !applyingSeek) advancePlayback();
	});
	connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 p) {
		if (!pendingPlay || awaitingMedia || applyingSeek || transitionPending || activeClip < 0 || activeClip >= clips.size()) return;
		qint64 base = 0; for (int i = 0; i < activeClip; ++i) base += clips[i].out - clips[i].in;
		position = std::clamp(base + p - clips[activeClip].in, qint64(0), total());
		timeline->position = position; timeline->update(); clock->setText(stamp(position) + " / " + stamp(total()));
		if (!pendingPlay || player->playbackState() != QMediaPlayer::PlayingState) return;
		if (selectionPlayback && position >= qint64(end->value() * 1000)) {
			selectionPlayback = false; seek(qint64(end->value() * 1000)); return;
		}
		if (p >= clips[activeClip].out) advancePlayback();
	});
	connect(player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &message) { showError(QStringLiteral("미리보기 재생 실패: %1").arg(message)); });
	QTimer::singleShot(0, this, resizeTimeline); refresh();
}

EduToolEditor::~EduToolEditor()
{
	for (auto *process : {probe, encoder, frameDecoder}) if (process) { process->disconnect(this); process->kill(); process->waitForFinished(3000); }
}
qint64 EduToolEditor::total() const { qint64 result = 0; for (const auto &c : clips) result += c.out - c.in; return result; }
QString EduToolEditor::tool(const QString &name) const
{
	QString executable = name;
#ifdef _WIN32
	executable += ".exe";
#endif
	const auto bundled = QCoreApplication::applicationDirPath() + "/" + executable;
	return QFileInfo::exists(bundled) ? bundled : QStandardPaths::findExecutable(executable);
}
void EduToolEditor::showError(const QString &message) { status->setText(message); }
void EduToolEditor::setBusy(bool busy)
{
	for (auto *b : findChildren<QPushButton *>()) if (b->objectName() != "eduToolCancelMediaJob") b->setEnabled(!busy);
	list->setEnabled(!busy); start->setEnabled(!busy); end->setEnabled(!busy); recent->setEnabled(!busy); insertMode->setEnabled(!busy);
}
EduToolEditor::EditState EduToolEditor::snapshot() const
{
	return {clips, position, start->value(), end->value(), list->currentRow()};
}
void EduToolEditor::restore(const EditState &state)
{
	player->pause(); selectionPlayback = false; clips = state.clips; position = state.position;
	dirty = true; refresh(); start->setValue(state.from); end->setValue(state.to); list->setCurrentRow(state.row);
}
void EduToolEditor::checkpoint() { undo.push_back(snapshot()); if (undo.size() > 100) undo.removeFirst(); redo.clear(); dirty = true; player->pause(); }
void EduToolEditor::refresh()
{
	pendingPlay = false; player->pause(); list->clear();
	for (const auto &c : clips) list->addItem(QStringLiteral("%1\n%2 — %3\n%4×%5 · %6 FPS · %7 MB").arg(c.name, stamp(c.in), stamp(c.out)).arg(c.width).arg(c.height).arg(c.fps, 0, 'f', 2).arg(QFileInfo(c.path).size() / (1024.0 * 1024.0), 0, 'f', 1));
	start->setMaximum(total() / 1000.0); end->setMaximum(total() / 1000.0);
	timeline->clips = clips; timeline->length = total(); timeline->update(); seek(std::min(position, total()));
}
void EduToolEditor::applyPendingSeek()
{
	applyingSeek = true;
	if (!pendingPlay) player->pause();
	player->setPosition(pendingSeek);
	if (pendingPlay) player->play();
	applyingSeek = false;
}
void EduToolEditor::seek(qint64 milliseconds, bool play)
{
	++seekGeneration; transitionPending = false; pendingPlay = play;
	if (clips.isEmpty()) {
		applyingSeek = true; activeClip = -1; awaitingMedia = false; player->stop(); player->setSource(QUrl()); applyingSeek = false;
		stillFrame->frame = {}; stillFrame->update(); preview->setCurrentWidget(stillFrame);
		position = 0; clock->setText(stamp(0)); return;
	}
	position = std::clamp(milliseconds, qint64(0), total());
	qint64 base = 0; int index = 0;
	while (index + 1 < clips.size() && position >= base + clips[index].out - clips[index].in) { base += clips[index].out - clips[index].in; ++index; }
	pendingSeek = std::min(clips[index].out - 1, clips[index].in + position - base);
	activeClip = index;
	if (play) preview->setCurrentWidget(video); else renderPausedFrame();
	const auto source = QUrl::fromLocalFile(clips[index].path);
	if (player->source() != source) {
		awaitingMedia = true; player->setSource(source);
	} else if (!awaitingMedia) applyPendingSeek();
	timeline->position = position; timeline->update(); clock->setText(stamp(position) + " / " + stamp(total()));
}
void EduToolEditor::renderPausedFrame()
{
	const auto generation = seekGeneration;
	const auto path = clips[activeClip].path;
	const double fps = clips[activeClip].fps > 0 ? clips[activeClip].fps : 30;
	const auto offset = std::max(qint64(0), std::min(pendingSeek, clips[activeClip].duration - qint64(std::ceil(1000 / fps))));
	stillFrame->frame = {}; stillFrame->update(); preview->setCurrentWidget(stillFrame);
	QTimer::singleShot(60, this, [this, generation, path, offset]() {
		if (generation != seekGeneration || pendingPlay) return;
		if (frameDecoder) {
			frameDecoder->disconnect(this); frameDecoder->kill();
			connect(frameDecoder, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), frameDecoder, &QObject::deleteLater);
		}
		auto *process = new QProcess(this); frameDecoder = process;
		connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, process, generation](int code, QProcess::ExitStatus exit) {
			const auto frame = QImage::fromData(process->readAllStandardOutput(), "PNG");
			if (frameDecoder == process) frameDecoder = nullptr;
			process->deleteLater();
			if (generation != seekGeneration || pendingPlay) return;
			if (code || exit != QProcess::NormalExit || frame.isNull()) { showError(QStringLiteral("해당 위치의 영상 프레임을 읽을 수 없습니다.")); return; }
			stillFrame->frame = frame; stillFrame->update();
		});
		connect(process, &QProcess::errorOccurred, this, [this, process, generation](QProcess::ProcessError error) {
			if (error != QProcess::FailedToStart) return;
			if (frameDecoder == process) frameDecoder = nullptr;
			process->deleteLater();
			if (generation == seekGeneration) showError(QStringLiteral("정지 미리보기 디코더를 실행할 수 없습니다."));
		});
		process->start(tool("ffmpeg"), {"-nostdin", "-v", "error", "-ss", QString::number(offset / 1000.0, 'f', 3), "-i", path,
			"-map", "0:v:0", "-frames:v", "1", "-vf", "scale=1280:720:force_original_aspect_ratio=decrease", "-f", "image2pipe", "-c:v", "png", "pipe:1"});
		QTimer::singleShot(10000, process, [process]() { if (process->state() != QProcess::NotRunning) process->kill(); });
	});
}
void EduToolEditor::advancePlayback()
{
	if (transitionPending || activeClip < 0 || activeClip >= clips.size()) return;
	qint64 next = 0; for (int i = 0; i <= activeClip; ++i) next += clips[i].out - clips[i].in;
	transitionPending = true; const auto generation = seekGeneration;
	QTimer::singleShot(0, this, [this, next, generation]() {
		if (generation != seekGeneration) return;
		if (selectionPlayback && next >= qint64(end->value() * 1000)) { selectionPlayback = false; seek(qint64(end->value() * 1000)); }
		else seek(next, next < total());
	});
}
void EduToolEditor::toggleFullscreen()
{
	if (fullscreen) { fullscreen->reject(); return; }
	const auto sizes = previewSplitter->sizes();
	auto *dialog = new QDialog(this); fullscreen = dialog;
	dialog->setWindowTitle(QStringLiteral("편집 미리보기 — Esc로 돌아가기"));
	auto *layout = new QVBoxLayout(dialog); layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(preview);
	auto *back = new QPushButton(QStringLiteral("편집 화면으로 돌아가기 (Esc)"), dialog); layout->addWidget(back);
	connect(back, &QPushButton::clicked, dialog, &QDialog::reject);
	connect(dialog, &QDialog::finished, this, [this, dialog, sizes]() {
		previewSplitter->insertWidget(1, preview); preview->show(); previewSplitter->setSizes(sizes);
		fullscreen = nullptr; dialog->deleteLater();
	});
	dialog->showFullScreen();
}
void EduToolEditor::dragEnterEvent(QDragEnterEvent *e) { if (e->mimeData()->hasUrls() && !importing && !exporting) e->acceptProposedAction(); }
void EduToolEditor::dropEvent(QDropEvent *e)
{
	QStringList paths; for (const auto &url : e->mimeData()->urls()) if (url.isLocalFile()) paths << url.toLocalFile();
	importFiles(paths, clips.isEmpty()); e->acceptProposedAction();
}
bool EduToolEditor::confirmClose()
{
	if (importing || exporting) { showError(QStringLiteral("진행 중인 작업을 취소하거나 완료한 후 닫으세요.")); return false; }
	if (!dirty) return true;
	const auto answer = QMessageBox::question(this, QStringLiteral("편집 작업 저장"), QStringLiteral("편집 작업을 저장할까요?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
	if (answer == QMessageBox::Cancel) return false;
	if (answer == QMessageBox::Save) { saveProject(); return !dirty; }
	return true;
}
void EduToolEditor::importFiles(QStringList paths, bool replace)
{
	if (paths.isEmpty() || importing || exporting || (replace && !confirmClose())) return;
	paths.removeDuplicates(); importPaths = paths; imported.clear(); replaceImport = replace; importing = true; setBusy(true); probeNext();
}
void EduToolEditor::importFailed(const QString &reason)
{
	if (probe) { probe->disconnect(this); probe->kill(); probe->deleteLater(); probe = nullptr; }
	importing = false; imported.clear(); importPaths.clear(); setBusy(false); showError(reason);
	pendingProject = {};
}
void EduToolEditor::probeNext()
{
	if (importPaths.isEmpty()) {
		if (!pendingProject.isEmpty()) {
			QVector<EduToolClip> restored;
			for (const auto &value : pendingProject["clips"].toArray()) {
				const auto saved = EduToolClip::fromJson(value.toObject());
				auto it = std::find_if(imported.begin(), imported.end(), [&saved](const EduToolClip &c) { return c.path == saved.path; });
				if (it == imported.end() || saved.out > it->duration) { importFailed(QStringLiteral("원본 길이가 작업 구간과 다릅니다. 기존 작업을 유지합니다.")); return; }
				auto clip = *it; clip.name = saved.name; clip.in = saved.in; clip.out = saved.out; restored.push_back(clip);
			}
			clips = restored; undo.clear(); redo.clear(); projectPath = pendingProject["projectPath"].toString(); dirty = pendingProject["relinked"].toBool();
			position = qint64(std::clamp(pendingProject["position"].toDouble(), 0.0, double(total())));
			importing = false; setBusy(false); refresh(); start->setValue(pendingProject["from"].toDouble()); end->setValue(pendingProject["to"].toDouble());
			pendingProject = {}; imported.clear(); status->setText(QStringLiteral("편집 작업과 원본 정보를 불러왔습니다.")); return;
		}
		checkpoint();
		if (replaceImport) { clips = imported; projectPath.clear(); position = 0; }
		else {
			int index = clips.size();
			if (insertMode->currentIndex() == 1) index = 0;
			if (insertMode->currentIndex() == 2) {
				qint64 offset = 0; index = 0;
				while (index < clips.size() && offset + clips[index].out - clips[index].in <= position) { offset += clips[index].out - clips[index].in; ++index; }
				if (index < clips.size() && position > offset) {
					auto tail = clips[index]; tail.in += position - offset; clips[index].out = tail.in; clips.insert(index + 1, tail); ++index;
				}
			}
			for (const auto &c : imported) clips.insert(index++, c);
		}
		QSettings settings("EduTool", "Studio"); auto recentPaths = settings.value("editor/recent").toStringList();
		for (const auto &c : imported) { recentPaths.removeAll(c.path); recentPaths.prepend(c.path); }
		while (recentPaths.size() > 12) recentPaths.removeLast(); settings.setValue("editor/recent", recentPaths);
		recent->clear(); recent->addItem(QStringLiteral("최근 영상")); for (const auto &p : recentPaths) recent->addItem(p, p);
		importing = false; imported.clear(); setBusy(false); refresh(); status->setText(QStringLiteral("영상을 불러왔습니다. 조각 이름은 두 번 클릭, 순서는 오른쪽 클릭으로 변경합니다.")); return;
	}
	const QString path = QFileInfo(importPaths.takeFirst()).absoluteFilePath();
	if (!QFileInfo(path).isFile() || tool("ffprobe").isEmpty()) { importFailed(QStringLiteral("영상 또는 ffprobe 도구를 찾을 수 없습니다: %1").arg(path)); return; }
	status->setText(QStringLiteral("영상 정보를 읽는 중: %1").arg(QFileInfo(path).fileName()));
	probe = new QProcess(this); auto *process = probe;
	connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) { importFailed(QStringLiteral("영상 분석 도구를 실행하지 못했습니다.")); });
	connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, process, path](int code, QProcess::ExitStatus state) {
		const auto object = QJsonDocument::fromJson(process->readAllStandardOutput()).object();
		process->deleteLater(); probe = nullptr;
		EduToolClip clip; clip.path = path; clip.name = QFileInfo(path).completeBaseName();
		const double seconds = object["format"].toObject()["duration"].toString().toDouble();
		if (std::isfinite(seconds) && seconds > 0 && seconds < 604800) clip.duration = clip.out = qint64(seconds * 1000);
		for (const auto &value : object["streams"].toArray()) {
			const auto stream = value.toObject();
			if (stream["codec_type"] == "audio") clip.audio = true;
			if (stream["codec_type"] == "video" && !clip.width) {
				clip.width = stream["width"].toInt(); clip.height = stream["height"].toInt();
				const auto rate = stream["avg_frame_rate"].toString().split('/');
				if (rate.size() == 2 && rate[1].toDouble() > 0) clip.fps = rate[0].toDouble() / rate[1].toDouble();
			}
		}
		if (code || state != QProcess::NormalExit || clip.duration <= 0 || clip.width <= 0 || clip.height <= 0) { importFailed(QStringLiteral("읽을 수 없는 영상입니다. 기존 작업을 유지합니다: %1").arg(path)); return; }
		if (tool("ffmpeg").isEmpty()) { importFailed(QStringLiteral("영상 디코더를 찾을 수 없습니다.")); return; }
		auto *decoder = new QProcess(this); probe = decoder;
		connect(decoder, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) { importFailed(QStringLiteral("미리보기 디코더를 실행할 수 없습니다.")); });
		connect(decoder, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, decoder, clip](int result, QProcess::ExitStatus exit) {
			const auto image = QImage::fromData(decoder->readAllStandardOutput(), "PNG");
			decoder->deleteLater(); probe = nullptr;
			if (result || exit != QProcess::NormalExit || image.isNull()) { importFailed(QStringLiteral("영상 프레임을 읽을 수 없습니다. 기존 작업을 유지합니다: %1").arg(clip.path)); return; }
			timeline->thumbnails.insert(clip.path, image); imported.push_back(clip); probeNext();
		});
		decoder->start(tool("ffmpeg"), {"-nostdin", "-v", "error", "-i", clip.path, "-map", "0:v:0", "-frames:v", "1", "-vf", "scale=160:-2", "-f", "image2pipe", "-c:v", "png", "pipe:1"});
		QTimer::singleShot(30000, decoder, [this, decoder]() { if (probe == decoder && decoder->state() != QProcess::NotRunning) importFailed(QStringLiteral("프레임 가져오기 시간이 초과되었습니다.")); });
	});
	process->start(tool("ffprobe"), {"-v", "error", "-show_entries", "format=duration:stream=codec_type,width,height,avg_frame_rate", "-of", "json", path});
	QTimer::singleShot(30000, process, [this, process]() { if (probe == process && process->state() != QProcess::NotRunning) importFailed(QStringLiteral("영상 분석 시간이 초과되었습니다.")); });
}
void EduToolEditor::split()
{
	if (importing || exporting) return;
	qint64 offset = 0;
	for (int i = 0; i < clips.size(); ++i) {
		const auto length = clips[i].out - clips[i].in;
		if (position > offset && position < offset + length) { checkpoint(); auto right = clips[i]; right.in += position - offset; clips[i].out = right.in; right.name += QStringLiteral(" (2)"); clips.insert(i + 1, right); refresh(); return; }
		offset += length;
	}
	showError(QStringLiteral("조각의 시작·끝에서는 나눌 수 없습니다."));
}
void EduToolEditor::removeSelection()
{
	if (importing || exporting) return;
	const qint64 from = qint64(start->value() * 1000), to = qint64(end->value() * 1000);
	if (from >= to || to > total()) { showError(QStringLiteral("유효한 시작·끝 시간을 선택하세요.")); return; }
	checkpoint(); QVector<EduToolClip> result; qint64 offset = 0;
	for (const auto &clip : clips) {
		const auto length = clip.out - clip.in;
		if (offset + length <= from || offset >= to) result.push_back(clip);
		else {
			if (from > offset) { auto left = clip; left.out = clip.in + from - offset; result.push_back(left); }
			if (to < offset + length) { auto right = clip; right.in = clip.in + to - offset; result.push_back(right); }
		}
		offset += length;
	}
	clips = result; position = from; selectionPlayback = false; start->setValue(0); end->setValue(0); refresh();
}
void EduToolEditor::saveProject()
{
	if (importing || exporting) return;
	const auto path = EduToolFileBrowser::saveFile(this, QStringLiteral("편집 작업 저장"), projectPath, ".edutool.json");
	if (path.isEmpty()) return;
	for (const auto &clip : clips) if (!QFileInfo(path).canonicalFilePath().isEmpty() &&
		QFileInfo(path).canonicalFilePath().compare(QFileInfo(clip.path).canonicalFilePath(), Qt::CaseInsensitive) == 0) {
		showError(QStringLiteral("원본 영상에 작업 파일을 덮어쓸 수 없습니다.")); return;
	}
	QJsonArray array; for (const auto &c : clips) array.append(c.json());
	QSaveFile file(path); const QByteArray data = QJsonDocument(QJsonObject{{"version", 1}, {"clips", array}, {"position", position}, {"from", start->value()}, {"to", end->value()}}).toJson();
	if (!file.open(QIODevice::WriteOnly) || file.write(data) != data.size() || !file.commit()) { showError(QStringLiteral("작업 저장 실패: %1").arg(file.errorString())); return; }
	projectPath = path; dirty = false; status->setText(QStringLiteral("편집 작업을 저장했습니다. 원본 파일은 이동하지 마세요."));
}
void EduToolEditor::loadProject()
{
	if (importing || exporting || !confirmClose()) return;
	const auto path = EduToolFileBrowser::openFiles(this, QStringLiteral("편집 작업 열기"), {}, true).value(0); if (path.isEmpty()) return;
	QFile file(path); if (!file.open(QIODevice::ReadOnly) || file.size() > 8 * 1024 * 1024) { showError(QStringLiteral("작업 파일을 읽을 수 없습니다.")); return; }
	auto object = QJsonDocument::fromJson(file.readAll()).object(); QJsonArray restored; QStringList paths; bool relinked = false;
	if (object["version"].toInt() != 1 || !object["clips"].isArray()) { showError(QStringLiteral("지원하지 않는 작업 파일입니다.")); return; }
	for (const auto &value : object["clips"].toArray()) {
		for (const auto &key : {"in", "out", "duration"}) {
			const double time = value.toObject()[key].toDouble(-1);
			if (!std::isfinite(time) || time < 0 || time > 604800000) { showError(QStringLiteral("잘못된 작업 시간 정보입니다.")); return; }
		}
		auto c = EduToolClip::fromJson(value.toObject());
		if (c.in < 0 || c.out <= c.in || c.out > c.duration || c.duration > 604800000 || c.width <= 0 || c.height <= 0) { showError(QStringLiteral("손상된 구간 정보입니다.")); return; }
		if (!QFileInfo(c.path).isFile()) {
			c.path = EduToolFileBrowser::openFiles(this, QStringLiteral("누락 원본 다시 연결: %1").arg(c.name)).value(0);
			if (c.path.isEmpty()) { showError(QStringLiteral("다시 연결을 취소했습니다. 기존 작업을 유지합니다.")); return; }
			relinked = true;
		}
		c.path = QFileInfo(c.path).absoluteFilePath(); restored.append(c.json()); paths << c.path;
	}
	object["clips"] = restored; object["projectPath"] = path; object["relinked"] = relinked;
	pendingProject = object; paths.removeDuplicates(); importPaths = paths; imported.clear(); importing = true; setBusy(true); probeNext();
}
void EduToolEditor::exportVideo(bool selectedOnly)
{
	if (exporting || importing || clips.isEmpty()) return;
	if (tool("ffmpeg").isEmpty()) { showError(QStringLiteral("ffmpeg 도구가 설치되어 있지 않습니다.")); return; }
	renderClips = clips;
	if (selectedOnly) { const int row = list->currentRow(); if (row < 0) { showError(QStringLiteral("저장할 조각을 목록에서 선택하세요.")); return; } renderClips = {clips[row]}; }
	exportPath = EduToolFileBrowser::saveFile(this, QStringLiteral("영상 내보내기 — 1280×720, 30 FPS, H.264/AAC"), {}, ".mp4");
	if (exportPath.isEmpty()) return;
	if (!exportPath.endsWith(".mp4", Qt::CaseInsensitive)) exportPath += ".mp4";
	if (QFileInfo::exists(exportPath)) { showError(QStringLiteral("기존 파일을 덮어쓰지 않습니다. 새 이름을 선택하세요.")); return; }
	temporary = std::make_unique<QTemporaryDir>(QFileInfo(exportPath).absolutePath() + "/.edutool-export-XXXXXX");
	if (!temporary->isValid()) { showError(QStringLiteral("내보낼 폴더에 쓸 수 없습니다.")); temporary.reset(); return; }
	exporting = true; abortExport = false; renderingConcat = false; renderIndex = 0; renderFiles.clear(); player->pause(); setBusy(true);
	renderNext();
}
void EduToolEditor::renderNext()
{
	if (abortExport) { finishExport(false, QStringLiteral("내보내기를 취소했습니다.")); return; }
	QStringList args = {"-hide_banner", "-nostdin", "-v", "error", "-progress", "pipe:1", "-n"};
	QString output;
	if (renderIndex < renderClips.size()) {
		const auto &clip = renderClips[renderIndex]; output = temporary->path() + QStringLiteral("/part-%1.mp4").arg(renderIndex);
		args << "-ss" << QString::number(clip.in / 1000.0, 'f', 3) << "-i" << clip.path;
		if (!clip.audio) args << "-f" << "lavfi" << "-i" << "anullsrc=r=48000:cl=stereo";
		args << "-t" << QString::number((clip.out - clip.in) / 1000.0, 'f', 3) << "-map" << "0:v:0" << "-map" << (clip.audio ? "0:a:0" : "1:a:0")
			<< "-vf" << "scale=1280:720:force_original_aspect_ratio=decrease,pad=1280:720:(ow-iw)/2:(oh-ih)/2,setsar=1,fps=30,setpts=PTS-STARTPTS"
			<< "-af" << "aresample=48000,asetpts=PTS-STARTPTS,apad" << "-c:v" << "libx264" << "-preset" << "fast" << "-crf" << "20"
			<< "-pix_fmt" << "yuv420p" << "-c:a" << "aac" << "-ac" << "2" << "-ar" << "48000" << "-movflags" << "+faststart" << output;
	} else {
		renderingConcat = true; output = temporary->path() + "/result.mp4";
		QFile concat(temporary->path() + "/clips.txt"); if (!concat.open(QIODevice::WriteOnly)) { finishExport(false, QStringLiteral("임시 작업 파일을 만들 수 없습니다.")); return; }
		for (int i = 0; i < renderFiles.size(); ++i) concat.write(QStringLiteral("file 'part-%1.mp4'\n").arg(i).toUtf8()); concat.close();
		args << "-f" << "concat" << "-safe" << "1" << "-i" << concat.fileName() << "-c" << "copy" << "-movflags" << "+faststart" << output;
	}
	encoder = new QProcess(this); auto *process = encoder; errorTail.clear();
	connect(process, &QProcess::readyReadStandardError, this, [this, process]() { errorTail = (errorTail + QString::fromUtf8(process->readAllStandardError())).right(3000); });
	connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
		const auto progress = QString::fromUtf8(process->readAllStandardOutput());
		QString elapsed;
		for (const auto &line : progress.split('\n')) if (line.startsWith("out_time=")) elapsed = line.mid(9).trimmed().left(8);
		status->setText(renderingConcat ? QStringLiteral("완성 영상을 저장하고 있습니다.") : QStringLiteral("영상 내보내기 %1/%2%3").arg(std::min(renderIndex + 1, int(renderClips.size()))).arg(renderClips.size()).arg(elapsed.isEmpty() ? QString() : QStringLiteral(" · 처리 시간 %1").arg(elapsed)));
	});
	connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
		if (error == QProcess::FailedToStart) { encoder = nullptr; process->deleteLater(); finishExport(false, QStringLiteral("영상 변환 도구 실행 실패")); }
	});
	connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, process, output](int code, QProcess::ExitStatus state) {
		encoder = nullptr; process->deleteLater();
		if (abortExport || code || state != QProcess::NormalExit || QFileInfo(output).size() <= 0) { finishExport(false, abortExport ? QStringLiteral("내보내기를 취소했습니다.") : QStringLiteral("내보내기 실패: %1").arg(errorTail)); return; }
		if (renderingConcat) {
			if (QFileInfo::exists(exportPath) || !QFile::rename(output, exportPath)) { finishExport(false, QStringLiteral("결과 파일을 저장할 수 없습니다. 기존 파일과 권한을 확인하세요.")); return; }
			finishExport(true, QStringLiteral("내보내기 완료: %1").arg(exportPath)); return;
		}
		renderFiles << output; ++renderIndex; renderNext();
	});
	process->start(tool("ffmpeg"), args);
}
void EduToolEditor::finishExport(bool success, const QString &message)
{
	exporting = false; temporary.reset(); setBusy(false); status->setText(message);
	if (success) emit exportReady(exportPath);
}
