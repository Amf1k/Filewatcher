#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "filewatchermodel.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);
  app.setApplicationName("FileWatcher");
  app.setOrganizationName("Amf1k");
  app.setOrganizationName("ru.Amf1k");

  // before create QQmlApplicationEngine
  Amf1k::FileWatcherModel::declareQML();

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
