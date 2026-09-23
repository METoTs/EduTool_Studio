#pragma once
#include <QWidget>
#include <QStringList>

class QFileSystemModel;
class QTreeView;
class QLineEdit;
class QLabel;

class EduToolFileBrowser : public QWidget {
	Q_OBJECT
public:
	explicit EduToolFileBrowser(QWidget *parent = nullptr);
	void navigate(const QString &path);
	QString directory() const;
	QStringList selectedFiles() const;
	static QStringList openFiles(QWidget *parent, const QString &title, const QString &path = {}, bool projects = false);
	static QString saveFile(QWidget *parent, const QString &title, const QString &path, const QString &suffix);
	static QString selectDirectory(QWidget *parent, const QString &title, const QString &path);
signals:
	void filesActivated(const QStringList &paths);
private:
	QFileSystemModel *model;
	QTreeView *tree;
	QLineEdit *address;
	QLabel *info;
};
