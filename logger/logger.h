#ifndef LOGGER_H
#define LOGGER_H
#include <QDebug>
#include <QDateTime>

#define LOG Logger::Print

class Logger
{
public:
    Logger();
    ~Logger();
    static inline void Print(const char *tag, const char *content)
    {
        const char *time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss.zzz").toUtf8().constData();
        qDebug() << time << "[" << tag << "]:" << content;
    }

    template <typename T>
    static inline void Print(const char *tag, const char *content, T variable)
    {
        const char *time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss.zzz").toUtf8().constData();
        qDebug() << time << "[" << tag << "]:" << content << variable;
    }

    static inline void Print(const char *tag, const char *content, char *variable)
    {
        const char *time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss.zzz").toUtf8().constData();
        qDebug() << time << "[" << tag << "]:" << content << variable;
    }

    static inline void Print(const char *tag, const char *content, QString variable)
    {
        const char *time = QDateTime::currentDateTime().toString("yyyy年MM月dd日|hh:mm:ss.zzz").toUtf8().constData();
        qDebug() << time << "[" << tag << "]:" << content << variable;
    }

};

#endif // LOGGER_H
