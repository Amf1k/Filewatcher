#include "filewatchermodel.hpp"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QTimer>
#include <chrono>
#include <directorywatcher.hpp>
#include <subscriber.hpp>

using namespace fwlib;

const QLoggingCategory FileWatcherModelCategory("FileWatcherModel");

Amf1k::FileWatcherModel::FileWatcherModel(QObject* parent)
    : QAbstractListModel(parent),
      _pDirectoryWatcher(new DirectoryWatcher),
      _files({}) {
  connect(this, &FileWatcherModel::currentWatchPathChanged, this,
          &FileWatcherModel::handleCurrentWatchPathChanged);
  _pDirectoryWatcher->subscribe(std::make_shared<Subscriber<FileWatcherModel>>(
      *this, &FileWatcherModel::handleFileInfoChanged));
  connect(this, &FileWatcherModel::newFileInfoEvenet, this,
          &FileWatcherModel::handleNewFileInfoEvent, Qt::QueuedConnection);
}

QString Amf1k::FileWatcherModel::getCurrentWatchPath() const {
  return _currentWatchPath;
}

void Amf1k::FileWatcherModel::setCurrentWatchPath(QUrl uri) {
  if (auto newPath = uri.toLocalFile(); newPath != _currentWatchPath) {
    _currentWatchPath = newPath;
    emit currentWatchPathChanged();
    qInfo(FileWatcherModelCategory)
        << "Choose new directory for watching" << _currentWatchPath;
  }
}

void Amf1k::FileWatcherModel::rename(QString fileName, QString newFileName) {
  //Такой себе механизм, по имени файла, он ведь может поменяться во время
  //изменения, но....
  if (bool result = QFile::rename(_currentWatchPath + "/" + fileName,
                                  _currentWatchPath + "/" + newFileName);
      result) {
    qInfo(FileWatcherModelCategory)
        << "Success rename file from" << fileName << "to" << newFileName;
  } else {
    qInfo(FileWatcherModelCategory)
        << "Failed rename file from" << fileName << "to" << newFileName;
  }
}

void Amf1k::FileWatcherModel::handleCurrentWatchPathChanged() {
  auto paths = _pDirectoryWatcher->paths();
  for (auto&& path : paths) {
    if (!_pDirectoryWatcher->removePath(path)) {
      qWarning(FileWatcherModelCategory)
          << "Failed remove path from watcher" << QString::fromStdString(path);
    }
  }
  if (!_pDirectoryWatcher->addPath(_currentWatchPath.toStdString())) {
    qWarning(FileWatcherModelCategory)
        << "Failed add path to watch" << _currentWatchPath;
  }
  auto newFiles = _pDirectoryWatcher->files();
  beginResetModel();
  _files.clear();
  _files.reserve(newFiles.size());
  std::copy(newFiles.begin(), newFiles.end(), std::back_inserter(_files));
  endResetModel();
}

void Amf1k::FileWatcherModel::handleFileInfoChanged(FileInfo info,
                                                    FileAction action) {
  //Глупый хак, нужно для нормальной работы с Qt EventLoop, иначе модель не
  //обновляется нормально. Для более красивого решения нужно сделать
  //библиотеку-враппер Qt над С++ pure библиотеки. И там разрулить эту ситуацию
  //с коннектами и тд
  emit newFileInfoEvenet(info, action);
}

void Amf1k::FileWatcherModel::handleFileRolesChanging(
    const QVector<int>& changedRoles,
    int row) {
  auto index = createIndex(row, 0);
  emit dataChanged(index, index, changedRoles);
}

void Amf1k::FileWatcherModel::handleNewFileInfoEvent(FileInfo info,
                                                     FileAction action) {
  qInfo(FileWatcherModelCategory) << "File changing";
  if (action != FileAction::Create) {
    auto it = std::find_if(_files.begin(), _files.end(),
                           [& newInfo = info](const auto& info) {
                             return info.fileName == newInfo.fileName;
                           });
    if (it == _files.end()) {
      qCritical(FileWatcherModelCategory)
          << "We have caught modifying or deleting a file we are not tracking"
             ": "
          << _currentWatchPath << QString::fromStdString(info.fileName);
      return;
    }
    if (action == FileAction::Delete) {
      qInfo(FileWatcherModelCategory)
          << "Delete file from watch:" << QString::fromStdString(info.fileName);
      deleteFileInfo(it);
    } else {
      qInfo(FileWatcherModelCategory)
          << "Update watching file" << QString::fromStdString(info.fileName)
          << "new size:" << info.size << "old size:" << (*it).size;
      updateFileInfo(info, it);
      return;
    }
  } else {
    qInfo(FileWatcherModelCategory)
        << "Added new file for watch:" << QString::fromStdString(info.fileName);
    appendFileInfo(info);
    return;
  }
}

int Amf1k::FileWatcherModel::rowCount(const QModelIndex&) const {
  return _files.size();
}

QVariant Amf1k::FileWatcherModel::data(const QModelIndex& index,
                                       int role) const {
  using namespace std::chrono;
  if (!index.isValid())
    return QVariant();
  auto fileInfo = _files.value(index.row());

  switch (role) {
    case FileNameRole:
      return QString::fromStdString(fileInfo.fileName);
    case FileModifiedDateRole:
      return QDateTime::fromTime_t(fileInfo.toTimestamp())
          .toString(Qt::DateFormat::ISODate);
    case FileSizeByte:
      return fileInfo.size;
  }
  return QVariant();
}

QHash<int, QByteArray> Amf1k::FileWatcherModel::roleNames() const {
  static QHash<int, QByteArray> roles = {{FileNameRole, "fileName"},
                                         {FileModifiedDateRole, "fileModified"},
                                         {FileSizeByte, "fileSizeByte"}};

  return roles;
}

void Amf1k::FileWatcherModel::updateFileInfo(FileInfo newInfo,
                                             QList<FileInfo>::Iterator it) {
  (*it) = std::move(newInfo);
  handleFileRolesChanging({FileNameRole, FileModifiedDateRole, FileSizeByte},
                          toRow(it));
}

void Amf1k::FileWatcherModel::deleteFileInfo(QList<FileInfo>::Iterator it) {
  beginRemoveRows(QModelIndex(), toRow(it), toRow(it));
  _files.removeOne(*it);
  endRemoveRows();
}

void Amf1k::FileWatcherModel::appendFileInfo(FileInfo newInfo) {
  beginInsertRows(QModelIndex(), _files.size(), _files.size());
  _files.append(std::move(newInfo));
  endInsertRows();
}

int Amf1k::FileWatcherModel::toRow(QList<FileInfo>::Iterator it) {
  if (int row = _files.indexOf(*it); row != -1) {
    qInfo(FileWatcherModelCategory) << "convert iterator to row" << row;
    return row;
  } else {
    qCritical(FileWatcherModelCategory)
        << "Failed convert iterator to row, return 0";
    return 0;
  }
}
