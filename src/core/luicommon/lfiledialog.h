#ifndef LFILEDIALOG_H
#define LFILEDIALOG_H
#include "ldialog.h"
#include <QFileDialog>
#include <QUrl>
struct LFileDialogArgs;
class LUICOMMON_EXPORT LFileDialog : public LDialog
{
	Q_OBJECT
public:
	explicit LFileDialog(QWidget *parent = nullptr,
						 const QString &caption = QString(),
						 const QString &directory = QString(),
						 const QString &filter = QString());
	virtual ~LFileDialog() override;

	QFileDialog  *fileDialog() const;
	virtual QSize sizeHint() const override;

public:
	static QString getOpenFileName(QWidget *parent = nullptr,
								   const QString &caption = QString(),
								   const QString &dir = QString(),
								   const QString &filter = QString(),
								   QString *selectedFilter = nullptr,
								   QFileDialog::Options options = QFileDialog::Options());

	static QUrl getOpenFileUrl(QWidget *parent = nullptr,
							   const QString &caption = QString(),
							   const QUrl &dir = QUrl(),
							   const QString &filter = QString(),
							   QString *selectedFilter = nullptr,
							   QFileDialog::Options options = QFileDialog::Options(),
							   const QStringList &supportedSchemes = QStringList());

	static QString getSaveFileName(QWidget *parent = nullptr,
								   const QString &caption = QString(),
								   const QString &dir = QString(),
								   const QString &filter = QString(),
								   QString *selectedFilter = nullptr,
								   QFileDialog::Options options = QFileDialog::Options());

	static QUrl getSaveFileUrl(QWidget *parent = nullptr,
                               const QString & caption = QString(),
                               const QUrl &dir = QUrl(),
                               const QString &filter = QString(),
                               QString * selectedFilter = nullptr,
                               QFileDialog::Options options = QFileDialog::Options(),
                               const QStringList & supportedSchemes = QStringList());

	static QString getExistingDirectory(QWidget *parent = nullptr,
										const QString &caption = QString(),
										const QString &dir = QString(),
										QFileDialog::Options options = QFileDialog::Options());

	static QUrl getExistingDirectoryUrl(QWidget *parent = nullptr,
									   const QString &caption = QString(),
									   const QUrl &dir = QUrl(),
									   QFileDialog::Options options = QFileDialog::Options(),
									   const QStringList& supportedSchemes = QStringList());

	static QStringList getOpenFileNames(QWidget *parent = nullptr,
		                                const QString &caption = QString(),
										const QString &dir = QString(),
										const QString &filter = QString(),
										QString *selectedFilter = nullptr,
										QFileDialog::Options options = QFileDialog::Options());

	static QList<QUrl> getOpenFileUrls(QWidget *parent = nullptr,
									   const QString &caption = QString(),
									   const QUrl &dir = QUrl(),
									   const QString &filter = QString(),
									   QString *selectedFilter = nullptr,
									   QFileDialog::Options options = QFileDialog::Options(),
									   const QStringList &supportedSchemes = QStringList());

protected:
	LFileDialog(const LFileDialogArgs &args);
	void init(const LFileDialogArgs& args);
private:
	QFileDialog *_fileDialog = nullptr;
};


#endif