#include "EduToolFileBrowser.hpp"
#include <QFileSystemModel>
#include <QTreeView>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QMessageBox>
#include <QKeyEvent>

class FolderAddressEdit : public QLineEdit {
public:
	using QLineEdit::QLineEdit;
protected:
	void keyPressEvent(QKeyEvent *event) override
	{
		QLineEdit::keyPressEvent(event);
		// returnPressed navigates, but must not propagate to QDialog's default button.
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
			event->accept();
	}
};

EduToolFileBrowser::EduToolFileBrowser(QWidget *parent) : QWidget(parent)
{
	setObjectName("eduToolFileBrowser");
	auto *layout = new QVBoxLayout(this);
	auto *bar = new QHBoxLayout;
	layout->addLayout(bar);
	auto *up = new QPushButton(QStringLiteral("상위 폴더"), this);
	auto *home = new QPushButton(QStringLiteral("동영상 폴더"), this);
	bar->addWidget(up); bar->addWidget(home);
	address = new FolderAddressEdit(this); address->setAccessibleName(QStringLiteral("탐색기 폴더 경로"));
	layout->addWidget(address);
	auto *filter = new QComboBox(this);
	filter->addItems({QStringLiteral("영상 파일"), QStringLiteral("편집 작업 (*.json)"), QStringLiteral("모든 파일")});
	filter->setAccessibleName(QStringLiteral("탐색기 파일 종류")); layout->addWidget(filter);
	model = new QFileSystemModel(this);
	model->setReadOnly(true);
	model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
	model->setNameFilterDisables(false);
	tree = new QTreeView(this); tree->setAccessibleName(QStringLiteral("프로젝트 파일 탐색기 목록"));
	tree->setModel(model); tree->setRootIsDecorated(false); tree->setItemsExpandable(false);
	tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tree->setDragEnabled(true); tree->setDragDropMode(QAbstractItemView::DragOnly);
	tree->setSortingEnabled(true); tree->sortByColumn(0, Qt::AscendingOrder);
	tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	tree->hideColumn(2); tree->hideColumn(3); tree->setColumnWidth(1, 90);
	layout->addWidget(tree, 1);
	info = new QLabel(this); info->setWordWrap(true); info->setTextFormat(Qt::PlainText); layout->addWidget(info);
	auto applyFilter = [this](int index) {
		model->setNameFilters(index == 0 ? QStringList{"*.mp4", "*.mkv", "*.mov", "*.avi", "*.webm", "*.wmv", "*.m4v", "*.ts"}
			: index == 1 ? QStringList{"*.json"} : QStringList{"*"});
	};
	connect(filter, &QComboBox::currentIndexChanged, this, applyFilter); applyFilter(0);
	connect(up, &QPushButton::clicked, this, [this]() { QDir dir(directory()); if (dir.cdUp()) navigate(dir.absolutePath()); });
	connect(home, &QPushButton::clicked, this, [this]() { navigate(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)); });
	connect(address, &QLineEdit::returnPressed, this, [this]() { navigate(address->text()); });
	connect(tree, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
		const QString path = model->filePath(index);
		if (model->isDir(index)) navigate(path); else emit filesActivated({path});
	});
	navigate(QSettings("EduTool", "Studio").value("files/lastDirectory", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString());
}

QString EduToolFileBrowser::directory() const { return model->filePath(tree->rootIndex()); }
QStringList EduToolFileBrowser::selectedFiles() const
{
	QStringList paths;
	for (const auto &index : tree->selectionModel()->selectedRows()) if (!model->isDir(index)) paths << model->filePath(index);
	return paths;
}
void EduToolFileBrowser::navigate(const QString &path)
{
	const QFileInfo entry(path);
	if (!entry.isDir() || !entry.isReadable()) { info->setText(QStringLiteral("폴더를 읽을 수 없습니다. 경로와 권한을 확인하세요.")); return; }
	const QString folder = entry.absoluteFilePath();
	model->setRootPath(folder); tree->setRootIndex(model->index(folder)); address->setText(QDir::toNativeSeparators(folder));
	QSettings("EduTool", "Studio").setValue("files/lastDirectory", folder);
	const QStorageInfo storage(folder);
	info->setText(QStringLiteral("폴더를 두 번 클릭해 이동 · 파일을 선택하거나 편집 화면으로 끌어놓기\n남은 공간: %1 GB")
		.arg(storage.bytesAvailable() / (1024.0 * 1024 * 1024), 0, 'f', 1));
}

static EduToolFileBrowser *setup(QDialog &dialog, const QString &title, const QString &path)
{
	dialog.setWindowTitle(title); dialog.resize(860, 620);
	auto *layout = new QVBoxLayout(&dialog);
	auto *browser = new EduToolFileBrowser(&dialog); layout->addWidget(browser);
	if (!path.isEmpty()) browser->navigate(QFileInfo(path).isDir() ? path : QFileInfo(path).absolutePath());
	return browser;
}

static void disableDefaultButtons(QDialogButtonBox *buttons)
{
	// Enter in the folder address navigates; it must not also accept the dialog.
	for (auto *button : buttons->findChildren<QPushButton *>()) {
		button->setAutoDefault(false);
		button->setDefault(false);
	}
}
QStringList EduToolFileBrowser::openFiles(QWidget *parent, const QString &title, const QString &path, bool projects)
{
	QDialog dialog(parent); auto *browser = setup(dialog, title, path);
	if (projects) browser->findChild<QComboBox *>()->setCurrentIndex(1);
	auto *buttons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel, &dialog); dialog.layout()->addWidget(buttons);
	disableDefaultButtons(buttons);
	QStringList result;
	auto accept = [&dialog, &result](const QStringList &paths) { if (!paths.isEmpty()) { result = paths; dialog.accept(); } };
	connect(browser, &EduToolFileBrowser::filesActivated, &dialog, accept);
	connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() { accept(browser->selectedFiles()); });
	connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	dialog.exec(); return result;
}
QString EduToolFileBrowser::selectDirectory(QWidget *parent, const QString &title, const QString &path)
{
	QDialog dialog(parent); auto *browser = setup(dialog, title, path);
	auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
	disableDefaultButtons(buttons);
	buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("현재 폴더 선택")); dialog.layout()->addWidget(buttons);
	connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	return dialog.exec() == QDialog::Accepted ? browser->directory() : QString();
}
QString EduToolFileBrowser::saveFile(QWidget *parent, const QString &title, const QString &path, const QString &suffix)
{
	QDialog dialog(parent); auto *browser = setup(dialog, title, path);
	auto *name = new QLineEdit(&dialog); name->setAccessibleName(QStringLiteral("저장 파일 이름"));
	name->setPlaceholderText(QStringLiteral("파일 이름을 입력하세요")); if (!path.isEmpty() && !QFileInfo(path).isDir()) name->setText(QFileInfo(path).fileName());
	dialog.layout()->addWidget(name);
	auto *message = new QLabel(&dialog); message->setWordWrap(true); dialog.layout()->addWidget(message);
	auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog); dialog.layout()->addWidget(buttons);
	disableDefaultButtons(buttons);
	QString result;
	connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
		QString file = name->text().trimmed();
		if (file.isEmpty() || file.contains('/') || file.contains('\\') || file.contains(':') || file == "." || file == "..") { message->setText(QStringLiteral("폴더는 위에서 선택하고 유효한 파일 이름을 입력하세요.")); return; }
		if (!file.endsWith(suffix, Qt::CaseInsensitive)) file += suffix;
		result = QDir(browser->directory()).filePath(file);
		if (QFileInfo::exists(result)) {
			if (suffix == ".mp4") { message->setText(QStringLiteral("기존 영상은 덮어쓸 수 없습니다. 다른 파일 이름을 입력하세요.")); result.clear(); return; }
			if (QMessageBox::question(&dialog, QStringLiteral("작업 저장"), QStringLiteral("기존 작업 파일을 바꾸시겠습니까?")) != QMessageBox::Yes) { result.clear(); return; }
		}
		dialog.accept();
	});
	connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
	return dialog.exec() == QDialog::Accepted ? result : QString();
}
