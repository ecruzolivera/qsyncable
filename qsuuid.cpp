#include "qsuuid.h"
#include <QRegularExpression>
#include <QUuid>
#include <QtQml>
/*! \qmltype Uuid
    \inqmlmodule QSyncable
    \brief Uuid Generator

 */

QSUuid::QSUuid(QObject *parent) : QObject(parent)
{

}

/*! \qmlmethod string Uuid::create()

  Call this function to generate a random Uuid in string format.
  It use the result from QUuid::createUuid().
 */

QString QSUuid::create() const
{
    return QUuid::createUuid().toString().replace(QRegularExpression("[{}]"), "");
}
