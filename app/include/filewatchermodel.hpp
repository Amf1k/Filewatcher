#ifndef FILEWATCHERMODEL_HPP
#define FILEWATCHERMODEL_HPP

#include <QAbstractListModel>
#include <QList>
#include <QSharedPointer>
#include <QtQml>
#include <fileinfo.hpp>

namespace fwlib {
class IDirectoryWatcher;
enum class FileAction;
}  // namespace fwlib

namespace Amf1k {
class FileWatcherModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QString currentWatchPath READ getCurrentWatchPath WRITE
                 setCurrentWatchPath NOTIFY currentWatchPathChanged)

 public:
  enum Roles {
    FileNameRole = Qt::UserRole + 1,
    FileModifiedDateRole,
    FileSizeByte
  };
  explicit FileWatcherModel(QObject* parent = nullptr);
  static void declareQML() {
    qmlRegisterType<FileWatcherModel>("amf1k.models", 1, 0, "FileWatcherModel");
  }

 public:
  QString getCurrentWatchPath() const;
  void setCurrentWatchPath(QUrl uri);
  Q_INVOKABLE void rename(QString fileName, QString newFileName);

 signals:
  void currentWatchPathChanged();
  void newFileInfoEvenet(fwlib::FileInfo info, fwlib::FileAction action);

 public slots:
  void handleCurrentWatchPathChanged();
  void handleFileInfoChanged(fwlib::FileInfo, fwlib::FileAction);
  void handleFileRolesChanging(const QVector<int>& changedRoles, int row);
  void handleNewFileInfoEvent(fwlib::FileInfo info, fwlib::FileAction action);

  // QAbstractItemModel interface
 public:
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

 private:
  void updateFileInfo(fwlib::FileInfo newInfo,
                      QList<fwlib::FileInfo>::Iterator it);
  void deleteFileInfo(QList<fwlib::FileInfo>::Iterator it);
  void appendFileInfo(fwlib::FileInfo newInfo);
  int toRow(QList<fwlib::FileInfo>::Iterator it);

 private:
  QString _currentWatchPath;
  //По хорошему, вынести в отдельную библиотеку враппер для pure C++ либы,
  //но не в этот раз
  QSharedPointer<fwlib::IDirectoryWatcher> _pDirectoryWatcher;
  QList<fwlib::FileInfo> _files;
};
}  // namespace Amf1k
Q_DECLARE_METATYPE(fwlib::FileInfo)
Q_DECLARE_METATYPE(fwlib::FileAction)

#endif  // FILEWATCHERMODEL_HPP
