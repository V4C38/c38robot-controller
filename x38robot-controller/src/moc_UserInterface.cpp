/****************************************************************************
** Meta object code from reading C++ file 'UserInterface.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/UserInterface.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UserInterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSUserInterfaceENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSUserInterfaceENDCLASS = QtMocHelpers::stringData(
    "UserInterface",
    "updateArmState",
    "",
    "std::vector<JointData>",
    "inJointData",
    "setJointData",
    "jointData",
    "updateAvailableSerialPorts",
    "ports",
    "setDriverState",
    "isActive",
    "onTabChanged",
    "index",
    "onSerialPortUpdateRequested",
    "onSerialPortSelected",
    "onEmergencyStopRequested",
    "onHomingSequenceRequested",
    "onTestRequested",
    "onSelectAxisAngle",
    "axis",
    "angle",
    "setDebugMode",
    "enabled",
    "driverSelected",
    "toggleDriverState"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSUserInterfaceENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   98,    2, 0x0a,    1 /* Public */,
       5,    1,  101,    2, 0x0a,    3 /* Public */,
       7,    1,  104,    2, 0x0a,    5 /* Public */,
       9,    1,  107,    2, 0x0a,    7 /* Public */,
      11,    1,  110,    2, 0x08,    9 /* Private */,
      13,    0,  113,    2, 0x08,   11 /* Private */,
      14,    1,  114,    2, 0x08,   12 /* Private */,
      15,    0,  117,    2, 0x08,   14 /* Private */,
      16,    0,  118,    2, 0x08,   15 /* Private */,
      17,    0,  119,    2, 0x08,   16 /* Private */,
      18,    2,  120,    2, 0x08,   17 /* Private */,
      21,    1,  125,    2, 0x08,   20 /* Private */,
      23,    1,  128,    2, 0x08,   22 /* Private */,
      24,    0,  131,    2, 0x08,   24 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, QMetaType::QStringList,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,   19,   20,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject UserInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSUserInterfaceENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSUserInterfaceENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSUserInterfaceENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UserInterface, std::true_type>,
        // method 'updateArmState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<JointData> &, std::false_type>,
        // method 'setJointData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<JointData> &, std::false_type>,
        // method 'updateAvailableSerialPorts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'setDriverState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSerialPortUpdateRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSerialPortSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onEmergencyStopRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onHomingSequenceRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTestRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectAxisAngle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'setDebugMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'driverSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'toggleDriverState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void UserInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UserInterface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateArmState((*reinterpret_cast< std::add_pointer_t<std::vector<JointData>>>(_a[1]))); break;
        case 1: _t->setJointData((*reinterpret_cast< std::add_pointer_t<std::vector<JointData>>>(_a[1]))); break;
        case 2: _t->updateAvailableSerialPorts((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 3: _t->setDriverState((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onSerialPortUpdateRequested(); break;
        case 6: _t->onSerialPortSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->onEmergencyStopRequested(); break;
        case 8: _t->onHomingSequenceRequested(); break;
        case 9: _t->onTestRequested(); break;
        case 10: _t->onSelectAxisAngle((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 11: _t->setDebugMode((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->driverSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->toggleDriverState(); break;
        default: ;
        }
    }
}

const QMetaObject *UserInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSUserInterfaceENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int UserInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
