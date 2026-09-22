#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QPointer>
#include <functional>

class QLineEdit;
class QCheckBox;
class QComboBox;
class QLabel;
class QProgressBar;
class QPushButton;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

// The deployment supplies the HTTPS adapter described in docs/edutool-server-contract.md.
class EduToolPortal : public QWidget {
	Q_OBJECT
public:
	explicit EduToolPortal(QWidget *parent = nullptr);
	~EduToolPortal();
	void offerFile(const QString &path);
	void recordingFinished(const QString &path);
	bool confirmClose();
signals:
	void accountChanged(const QString &name);
private:
	QNetworkAccessManager *network;
	QLineEdit *email, *password, *file, *title;
	QCheckBox *remember, *automatic, *notify, *removeOriginal;
	QComboBox *folders;
	QLabel *account, *status;
	QProgressBar *progress;
	QPushButton *upload;
	QUrl base;
	QByteArray token;
	QString currentFile, uploadKey, uploadId, expectedHash;
	qint64 expectedSize = 0;
	QPointer<QNetworkReply> active;
	QTimer *oauthPoll = nullptr;
	int generation = 0;
	bool busy = false;
	bool jobRemoveOriginal = false;
	QJsonObject oauthSession;
	QJsonArray queuedRecordings;
	void nextRecording();
	void configure();
	void login();
	void logout();
	void oauth(const QString &provider);
	void session(const QJsonObject &json);
	void identity(const QJsonObject &json);
	void loadFolders();
	void newFolder();
	void uploadFile();
	void sendUpload();
	void verifyUpload();
	void cancel();
	void message(const QString &text);
	void setBusy(bool value);
	void persistOptions();
	void persistJob(const QString &state);
	bool validBase(const QUrl &url) const;
	void request(const QString &method, const QString &path, const QJsonObject &body,
		     std::function<void(const QJsonObject &)> done);
	void handleFailure(QNetworkReply *reply);
};
