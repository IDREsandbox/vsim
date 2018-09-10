#include "WeakObject.h"


WeakObject::WeakObject(QObject *parent)
    : QObject(parent)
{
}

WeakObject::~WeakObject()
{
    QList<QObject*> all = children();
    for (auto *child : all) {
        child->setParent(nullptr);
    }
}