#ifndef _H_CSIPTERMCALL_SM
#define _H_CSIPTERMCALL_SM

/*
 * ex: set ro:
 * DO NOT EDIT.
 * generated by smc (http://smc.sourceforge.net/)
 * from file : CSipTermCall_sm.sm
 */


#define SMC_USES_IOSTREAMS

#include <statemap.h>

// Forward declarations.
class CSipTermCallState;
class CSipTermCallState_IDLE;
class CSipTermCallState_CALLPROC;
class CSipTermCallState_BEAR_GATEWAY_READY;
class CSipTermCallState_BEAR_CONFIRMING;
class CSipTermCallState_BEAR_CONFIRMED;
class CSipTermCallState_BEAR_CLIENT_READY;
class CSipTermCallState_BEAR_MODYFYING;
class CSipTermCallState_ACTIVE;
class CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING;
class CSipTermCallState_ACTIVE_WAIT_ACK;
class CSipTermCallState_CLOSED;
class CSipTermCallState_Default;
class CR2SCallModuleState_Sip;
class CSipTermCallContext;
class CR2SCallModule;

class CR2SCallModuleState_Sip :
    public statemap::State
{
public:

    CR2SCallModuleState_Sip(const char *name, int stateId)
    : statemap::State(name, stateId)
    {};

    virtual void Entry(CSipTermCallContext&) {};
    virtual void Exit(CSipTermCallContext&) {};

    virtual void onAck(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onBye(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onError(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onInvite(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onNotify(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
    virtual void onUpdate(CSipTermCallContext& context, TUniNetMsg* msg);

protected:

    virtual void Default(CSipTermCallContext& context);
};

class CSipTermCallState
{
public:

    static CSipTermCallState_IDLE IDLE;
    static CSipTermCallState_CALLPROC CALLPROC;
    static CSipTermCallState_BEAR_GATEWAY_READY BEAR_GATEWAY_READY;
    static CSipTermCallState_BEAR_CONFIRMING BEAR_CONFIRMING;
    static CSipTermCallState_BEAR_CONFIRMED BEAR_CONFIRMED;
    static CSipTermCallState_BEAR_CLIENT_READY BEAR_CLIENT_READY;
    static CSipTermCallState_BEAR_MODYFYING BEAR_MODYFYING;
    static CSipTermCallState_ACTIVE ACTIVE;
    static CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING ACTIVE_WAIT_BEAR_MODIFYING;
    static CSipTermCallState_ACTIVE_WAIT_ACK ACTIVE_WAIT_ACK;
    static CSipTermCallState_CLOSED CLOSED;
};

class CSipTermCallState_Default :
    public CR2SCallModuleState_Sip
{
public:

    CSipTermCallState_Default(const char *name, int stateId)
    : CR2SCallModuleState_Sip(name, stateId)
    {};

    virtual void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onNotify(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onError(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onAck(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onUpdate(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onInvite(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onBye(CSipTermCallContext& context, TUniNetMsg* msg);
    virtual void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_IDLE :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_IDLE(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onNotify(CSipTermCallContext& context, TUniNetMsg* msg);
};

class CSipTermCallState_CALLPROC :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_CALLPROC(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_BEAR_GATEWAY_READY :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_BEAR_GATEWAY_READY(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onError(CSipTermCallContext& context, TUniNetMsg* msg);
    void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
    void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
};

class CSipTermCallState_BEAR_CONFIRMING :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_BEAR_CONFIRMING(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_BEAR_CONFIRMED :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_BEAR_CONFIRMED(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
    void onUpdate(CSipTermCallContext& context, TUniNetMsg* msg);
};

class CSipTermCallState_BEAR_CLIENT_READY :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_BEAR_CLIENT_READY(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onError(CSipTermCallContext& context, TUniNetMsg* msg);
    void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_BEAR_MODYFYING :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_BEAR_MODYFYING(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onError(CSipTermCallContext& context, TUniNetMsg* msg);
    void onResponse(CSipTermCallContext& context, TUniNetMsg* msg);
    void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_ACTIVE :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_ACTIVE(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};
    void Entry(CSipTermCallContext&);
    void onBye(CSipTermCallContext& context, TUniNetMsg* msg);
    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onInvite(CSipTermCallContext& context, TUniNetMsg* msg);
    void onUpdate(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
    void Exit(CSipTermCallContext&);

};

class CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_ACTIVE_WAIT_BEAR_MODIFYING(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onBye(CSipTermCallContext& context, TUniNetMsg* msg);
    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onSdpAnswer(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_ACTIVE_WAIT_ACK :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_ACTIVE_WAIT_ACK(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void onAck(CSipTermCallContext& context, TUniNetMsg* msg);
    void onBye(CSipTermCallContext& context, TUniNetMsg* msg);
    void onClose(CSipTermCallContext& context, TUniNetMsg* msg);
    void onTimeOut(CSipTermCallContext& context, TTimeMarkExt timerMark);
};

class CSipTermCallState_CLOSED :
    public CSipTermCallState_Default
{
public:
    CSipTermCallState_CLOSED(const char *name, int stateId)
    : CSipTermCallState_Default(name, stateId)
    {};

    void Entry(CSipTermCallContext&);
};

class CSipTermCallContext :
    public statemap::FSMContext
{
public:

    CSipTermCallContext(CR2SCallModule& owner)
    : FSMContext(CSipTermCallState::IDLE),
      _owner(owner)
    {};

    CSipTermCallContext(CR2SCallModule& owner, const statemap::State& state)
    : FSMContext(state),
      _owner(owner)
    {};

    virtual void enterStartState()
    {
        getState().Entry(*this);
        return;
    }

    CR2SCallModule& getOwner() const
    {
        return (_owner);
    };

    CR2SCallModuleState_Sip& getState() const
    {
        if (_state == NULL)
        {
            throw statemap::StateUndefinedException();
        }

        return (dynamic_cast<CR2SCallModuleState_Sip&>(*_state));
    };

    void onAck(TUniNetMsg* msg)
    {
        (getState()).onAck(*this, msg);
    };

    void onBye(TUniNetMsg* msg)
    {
        (getState()).onBye(*this, msg);
    };

    void onClose(TUniNetMsg* msg)
    {
        (getState()).onClose(*this, msg);
    };

    void onError(TUniNetMsg* msg)
    {
        (getState()).onError(*this, msg);
    };

    void onInvite(TUniNetMsg* msg)
    {
        (getState()).onInvite(*this, msg);
    };

    void onNotify(TUniNetMsg* msg)
    {
        (getState()).onNotify(*this, msg);
    };

    void onResponse(TUniNetMsg* msg)
    {
        (getState()).onResponse(*this, msg);
    };

    void onSdpAnswer(TUniNetMsg* msg)
    {
        (getState()).onSdpAnswer(*this, msg);
    };

    void onTimeOut(TTimeMarkExt timerMark)
    {
        (getState()).onTimeOut(*this, timerMark);
    };

    void onUpdate(TUniNetMsg* msg)
    {
        (getState()).onUpdate(*this, msg);
    };

private:

    CR2SCallModule& _owner;
};


/*
 * Local variables:
 *  buffer-read-only: t
 * End:
 */

#endif // _H_CSIPTERMCALL_SM
