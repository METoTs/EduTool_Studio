#pragma once

#include <QWidget>
#include <QVector>
#include <QJsonObject>
#include <memory>

class QMediaPlayer;
class QAudioOutput;
class QVideoWidget;
class QListWidget;
class QLabel;
class QDoubleSpinBox;
class QSlider;
class QProcess;
class QComboBox;
class QTemporaryDir;
class EduToolTimeline;

struct EduToolClip {
	QString path, name;
	qint64 in = 0, out = 0, duration = 0;
	int width = 0, height = 0;
	double fps = 0;
	bool audio = false;
	QJsonObject json() const;
	static EduToolClip fromJson(const QJsonObject &json);
};

class EduToolEditor : public QWidget {
	Q_OBJECT
public:
	explicit EduToolEditor(QWidget *parent = nullptr);
	~EduToolEditor();
	bool confirmClose();
signals:
	void exportReady(const QString &path);
	void uploadRequested(const QString &path);
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
private:
	QVector<EduToolClip> clips;
	struct EditState {
		QVector<EduToolClip> clips;
		qint64 position;
		double from, to;
		int row;
	};
	QVector<EditState> undo, redo;
	EditState snapshot() const;
	void restore(const EditState &state);
	QMediaPlayer *player;
	QAudioOutput *audio;
	QVideoWidget *video;
	QListWidget *list;
	QLabel *status, *clock;
	QDoubleSpinBox *start, *end;
	QSlider *zoom;
	QComboBox *recent, *insertMode;
	EduToolTimeline *timeline;
	QProcess *probe = nullptr, *encoder = nullptr;
	QStringList importPaths;
	QJsonObject pendingProject;
	QVector<EduToolClip> imported;
	bool replaceImport = false, dirty = false, importing = false, exporting = false, selectionPlayback = false;
	bool renderingConcat = false, abortExport = false;
	int activeClip = -1, renderIndex = 0;
	qint64 position = 0, pendingSeek = 0;
	QString projectPath, exportPath, errorTail;
	std::unique_ptr<QTemporaryDir> temporary;
	QVector<EduToolClip> renderClips;
	QStringList renderFiles;
	qint64 total() const;
	void checkpoint();
	void refresh();
	void seek(qint64 milliseconds, bool play = false);
	void importFiles(QStringList paths, bool replace);
	void probeNext();
	void importFailed(const QString &reason);
	void split();
	void removeSelection();
	void saveProject();
	void loadProject();
	void exportVideo(bool selectedOnly);
	void renderNext();
	void finishExport(bool success, const QString &message);
	void setBusy(bool busy);
	QString tool(const QString &name) const;
	void showError(const QString &message);
};
