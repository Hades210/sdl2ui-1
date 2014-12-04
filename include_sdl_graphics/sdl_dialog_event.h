/*============================================================================*/
/**  @file       sdl_dialog_event.h
 **  @ingroup    zhongcan_user_interface
 **  @brief		 Default dialog.
 **
 **  Create an event for the dialogs.
 **
 **  @author     mensfort
 **
 **  @par Classes:
 **              Cevent
 */
/*------------------------------------------------------------------------------
 **  Copyright (c) Bart Houkes, 28 jan 2011
 ** 
 **  Copyright notice:
 **  This software is property of Bart Houkes.
 **  Unauthorized duplication and disclosure to third parties is forbidden.
 **============================================================================*/

/*------------- Standard includes --------------------------------------------*/

#pragma once

#include <string>
#include <SDL/SDL.h>
#include <pthread.h>
#include <queue>
#include <vector>

#include "sdl_font.h"
//#include "zhongcan_defines.h"
#include "sdl_dialog_object.h"
#include "my_thread.h"
#include "timeout.h"
#include "sdl_key_file.h"

/// @brief Status for message when sent to any client.
typedef enum
{
	DIALOG_EVENT_PROCESSED,		///< Event processed, stop sending.
	DIALOG_EVENT_OPEN,			///< Event still open.
	DIALOG_EVENT_GENERAL,		///< Dialog can be sent to all.
	DIALOG_EVENT_EXIT			///< Dialog request to stop the dialog.
} Estatus;

/// @brief Status for the mouse.
typedef enum
{
	MOUSE_RELEASED,				///< No activity.
	MOUSE_START_DRAG,			///< Wait a second before starting to drag.
	MOUSE_START_SCROLL_OR_DRAG,	///< Start a scroll dialog.
	MOUSE_SCROLL,				///< Scrolling window.
	MOUSE_DRAG,					///< Busy dragging.
	MOUSE_PRESS,				///< Press mouse without drag, possible repeat.
	MOUSE_DRAG_DIALOG,			///< Drag entire dialog.
} EmouseStatus;

/// @brief Status after polling.
typedef enum
{
	POLL_EMPTY,
	POLL_USER,
	POLL_TESTING
} EpollStatus;

typedef enum
{
	EVENT_TOUCH_PRESS,		///< Output event.
	EVENT_TOUCH_RELEASE,	///< Output event.
	EVENT_TOUCH_MOVE,   	///< Output event.
	EVENT_KEY_PRESS,		///< Input/Output event.
	EVENT_KEY_RELEASE,		///< Input event.
	EVENT_DRAG_START,		///< Input event.
	EVENT_DRAG_MOVE,		///< Output event.
	EVENT_MOUSE_MOVE,		////< Just move the mouse when pressed.
	EVENT_DRAG_STOP,		///< Input event.
	EVENT_APPMOUSEFOCUS,	///< Input/Output event.
	EVENT_APPMOUSEBLUR, 	///< Input/Output event.
	EVENT_APPINPUTFOCUS,	///< Input/Output event.
	EVENT_APPINPUTBLUR, 	///< Input/Output event.
	EVENT_RESTORE,      	///< Input/Output event.
	EVENT_MINIMIZE,     	///< Input/Output event.
	EVENT_LANGUAGE_CHANGE,  ///< Output event.
	EVENT_MAIN_DIALOG,		///< Output event.
	EVENT_BUTTON,			///< Output event.
	EVENT_MOUSE_CLICK,		///< Short click for a button.
	EVENT_WHEEL_DOWN,		///< Wheel rotate (mouse).
	EVENT_WHEEL_UP,			///< Wheel rotate (mouse).
	EVENT_QUIT,				///< Quit application.
	EVENT_INVALID,
} EventType;

/// @brief Definition of an event.
class Cevent
{
public:
	Cevent( SDL_Event *event, bool spaceIsLanguage, bool test);
	Cevent( EventType t, keybutton key, const Cpoint &p, bool testing)
	: type( t)
	, status( testing ? POLL_TESTING:POLL_USER)
	, point( p)
	, button( key)
	, mod( KMOD_NONE)
	, which( 0)
	{
	}
	Cevent( keybutton key, SDLMod m, bool testing=false)
	: type( EVENT_KEY_PRESS)
	, status( testing ? POLL_TESTING:POLL_USER)
	, point( 0,0)
	, button( key)
	, mod( m)
	, which( 0)
	{
	}
	Cevent( EventType t, const Cpoint &p, bool testing)
	: type(t)
	, status( testing ? POLL_TESTING:POLL_USER)
	, point( p)
	, button( KEY_NONE)
	, mod( KMOD_NONE)
	, which( 0)
	{
	}
	~Cevent() {}
	std::string toString();

public:
	EventType	type;
	EpollStatus status;
	Cpoint		point;
	keybutton   button;
	SDLMod      mod;
	int			which;
};

/// @brief Queue for events.
class CeventQueue
{
public:
	CeventQueue();
	~CeventQueue();
	void push_back( const Cevent &event);
	Cevent front();
	void pop_front();
	int size();
	bool empty();

private:
	std::queue<Cevent> m_eventList; ///< All events in queue.
	CmyLock m_lock; ///< Lock for this queue.
};

/// @brief Interface for keyboard/mouse events.
class CeventInterface
{
public:
	CeventInterface() {}
	virtual ~CeventInterface() {}

public:
	virtual Estatus onEvent( Cevent &event) =0;
	virtual void onStartDrag( CdialogObject * movingObject) =0;
	virtual void onStopDrag() =0;
	void addDialog( CeventInterface	*interface);
	void removeDialog( CeventInterface	*interface);
	void stopDialog( CeventInterface *interface);
	virtual CdialogObject *findObject( const Cpoint &p) =0;
	virtual keybutton findButton( const Cpoint &p) =0;
	//virtual Cdialog *findDialog( const Cpoint &p) =0;
	virtual bool isSwypeDialog( const Cpoint &p) =0;
	virtual void onMouseWheel(bool Up, bool Down) =0;
	virtual void scrollDialog( const Cpoint &start, const Cpoint &distance) =0;
	virtual void dragObject( CdialogObject * movingObject, const Cpoint &to) =0;
	virtual std::string getName() =0;
	virtual bool spaceIsLanguage() =0;
	virtual bool isScrollDragDialog( const Cpoint &p) =0;
	virtual bool isHorizontalScrollDialog( const Cpoint &p) =0;
	virtual void notifyTouch( const Cpoint &p)=0;
};

/// @brief Create events for a dialog on screen.
class CdialogEvent : public Tsingleton<CdialogEvent>, public CmyThread
{
	friend class Tsingleton<CdialogEvent>;

public:
	CdialogEvent();
	virtual ~CdialogEvent();
	EpollStatus pollEvent( CeventInterface *callback);
	void stop() { CmyThread::stop(); }
	virtual void work();
	void stopDrag();
	Cpoint lastMouse() { return m_lastMouse; }
	bool pressMouse() { return m_isPressed; }
	EmouseStatus getStatus() { return m_mouseStatus; }
	void startIdiot() { m_idiot =true; }
	void forceDrag( const Cpoint &dragPoint) { m_dragPoint =dragPoint; m_mouseStatus =MOUSE_DRAG; }
	void registerActiveDialog( CeventInterface *interface); //{ m_interface =interface; }
	CeventInterface *getInterface() { return m_interface; }

private:
	void handleEvent( Cevent &event);
	void createRandomEvent();
	void flushEvents();
	void handleKeyPress( SDLMod mod, keybutton key);
	void handleKeyRelease();
	void handleMousePress( const Cpoint &p);
	void handleMouseRelease( const Cpoint &p);
	void handleMouseMove( const Cpoint &p);

private:
	CeventInterface		*m_interface;		///< Which interface is active.
	EmouseStatus  		m_mouseStatus; 		///< Mouse status.
	int					m_minimumDragTime; 	///< Minimum time to start drag.
	int					m_minimumClickTime; ///< Minimum time to send click.
	int					m_numberOfPresses;	///< Number of press for button.
	Cpoint				m_lastMouse;		///< Last mouse position.
	int					m_repeatDelay;		///< Delay for repeat.
	int					m_repeatSpeed;		///< Speed for repeat.
	CeventQueue			m_events;			///< Events to send.
	int					m_repeatCount;		///< Time to repeat.
	Ctimeout 			m_keyPress;			///< Time to repeat.
	Ctimeout			m_mousePress;		///< Time to press mouse.
	keybutton			m_repeatButton;		///< Object event may be repeated.
	Ctimeout			m_repeatTimer; 		///< Timeout until next key press
	SDL_Event 			m_event;			///< New SDL event coming in...
	bool				m_mousePressedLeft; ///< Press mouse.
	Cpoint				m_scrollStart;		///< Start dragging in 800x600 res.
	Cpoint				m_dragPoint;		///< Dragging x-coordinate in 800x600 res.
	int					m_dragStartPixels;	///< Minimum distance to start drag.
	CkeyFile			m_keyFile;			///< Test class to remember all buttons today.
	Cpoint				m_press;			///< Where we press the screen.
	bool				m_isPressed;		///< Is the mouse pressed?
public:
	bool				m_idiot;			///< Idiot test.
};

