/*============================================================================*/
/**  @file       sdl_slider.cpp
 **  @ingroup    zhongcan_sdl
 **  @brief		 Default dialog.
 **
 **  Create a scrollbar. This is a dialog object.
 **
 **  @author     mensfort
 **
 **  @par Classes:
 **              Cslider
 */
/*------------------------------------------------------------------------------
 **  Copyright (c) Bart Houkes, 28 jan 2011
 **
 **  Copyright notice:
 **  This software is property of Bart Houkes.
 **  Unauthorized duplication and disclosure to third parties is forbidden.
 **============================================================================*/

/*------------- Standard includes --------------------------------------------*/

#include "sdl_button.h"
#include "log_base.h"
#include "sdl_slider.h"
#include "sdl_image.h"
#include "zhongcan_conf.h"
#include "graphics_widgets.h"
#include "global.h"
#include "zhongcan_icon.h"

#define TOUCH_SLIDER

/*============================================================================*/
///
/// @brief Constructor.
//
/// @param x [in] Position.
/// @param y [in] Position.
/// @param width [in] Position.
/// @param height [in] Position.
///
/*============================================================================*/
Cslider::Cslider( const Crect &rect, int unitSize, keybutton code, EsliderType type)
: CdialogObject( NULL, rect, code)
, m_sliderType( type)
, m_scrollbarColour( COLOUR("COLOUR_BUTTON_TEXT"))
, m_backgroundColour( COLOUR("COLOUR_BUTTON_BACKGROUND2"))
, m_noBackground(true)
, m_imageEnable(false)
, m_image_y(0)
, m_unitSize( unitSize)
, m_value(-1000)
, m_minimum(0)
, m_maximum(1.0)
, m_unit(6.0)
, m_offDisabled( false)
, m_itemsOnScreen( m_rect.height()/m_unitSize)
{
}

/*============================================================================*/
///
/// @brief Destructor.
///
/*============================================================================*/
Cslider::~Cslider()
{
	// TODO Auto-generated destructor stub
}

/*============================================================================*/
///
///  @brief 	Paint the dialog once. Use invalidate() to re-paint soon.
///
///	 @pre		Called when dialog is invalidated.
///
///  @post		Dialog updated. Buttons, drag and background are done automatic.
///
/*============================================================================*/
void Cslider::onPaint( int touch)
{
	(void)touch;
	if ( !m_visible)
	{
		return;
	}
	switch (m_sliderType)
	{
	case SLIDER_HORIZONTAL:
		break;

	case SLIDER_VERTICAL:
		paintVertical();
		break;

	case SLIDER_VERTICAL_UPDOWN:
		paintVertical();
		break;
	}
	enableDrag();
	// height_arrow=MAX(MIN((height*height)/(unit*(maximum-minimum)); height/2);unit)
	// offset_arrow=(height-2*height_arrow)*(y/(maximum-minimum))
}

#if 0
void Cslider::paintVerticalUpDown()
{
	int    maxHeight   =static_cast<int>(m_rect.height()*8/2);
	double heightArrow =(double)( m_rect.height()*m_rect.height())/(2*m_unit*(m_maximum-m_minimum))+0.5;
	if ( heightArrow>maxHeight)
	{
		heightArrow =maxHeight;
	}
	if ( heightArrow <m_unit)
	{
		heightArrow =m_unit;
	}
	int hh =static_cast<int>(heightArrow);
	double offset_arrow =(m_rect.height()-2*hh)*(m_value/(m_maximum-m_minimum-(m_rect.height()/m_unit)));
	int y1 =static_cast<int>(offset_arrow+0.5f+m_rect.top());
	if (y1<m_rect.top()) y1=m_rect.top();
	if (y1+hh+hh>m_rect.bottom()) y1=m_rect.bottom()-hh-hh;
	int y2 =y1+hh;
}
#endif

/// @brief Paint a vertical slider.
void Cslider::paintVertical()
{
	int  heightArrow =6;
	double  maxHeight =(m_rect.height()-heightArrow); // Image height removed.
	double offset_arrow =0;
	int slidingRange =m_maximum-m_minimum-m_itemsOnScreen;
	if ( slidingRange >0)
	{
		offset_arrow =maxHeight*8*(m_value-m_minimum) /(m_maximum-m_minimum-m_itemsOnScreen);
	}
	m_image_y=(int)offset_arrow+m_rect.top()*8;
	m_graphics->setColour( COLOUR("WHITE"));
	m_graphics->rectangle( m_rect.left()*8, m_rect.top()*8, m_rect.right()*8, m_rect.bottom()*8, m_rect.width()*4,m_rect.width()*3);
	m_graphics->setColour( COLOUR("BLACK"));
	int x=(m_rect.left()+m_rect.right())*8/2;
	int y1=m_rect.top()*8+24;
	int y2=m_rect.bottom()*8-24;
	m_graphics->line( x,y1, x,y2);
	m_graphics->line( x-10,y1, x+10,y1);
	m_graphics->line( x-5,(y1+y2)/2, x+5, (y1+y2)/2);
	m_graphics->line( x-10,y2, x+10,y2);

	int left =m_rect.left()*8+(int)m_rect.width()*8/2-24;
	m_graphics->image( ICON_SLIDER48, left, m_image_y, left+47, m_image_y+47);
}

/*============================================================================*/
///
/// @brief Paint background to button.
///
/*============================================================================*/
void Cslider::paintBackground()
{
}

/*============================================================================*/
///
/// @brief Paint border of button.
///
/*============================================================================*/
void Cslider::paintBorder()
{
}

/*============================================================================*/
///
/// @brief Paint image next to text.
///
/*============================================================================*/
void Cslider::paintImage()
{
}

/*============================================================================*/
///
/// @brief Set new text.
///
/// @param text [in] Text to display.
///
/*============================================================================*/
void Cslider::setImage( const std::string &image)
{
	m_image =image;
	m_imageEnable =(m_image.size() >0);
}

/*============================================================================*/
///
/// @brief Set new text.
///
/// @param text [in] Text to display.
///
/*============================================================================*/
void Cslider::setColours( colour scrollbar, colour background)
{
	(void)scrollbar;
	m_backgroundColour =background;
}

bool Cslider::wheelUp()
{
	m_value =m_value-(m_itemsOnScreen-1.01);
	if ( m_value<m_minimum)
	{
		m_value =m_minimum;
	}
	return true;
}

bool Cslider::wheelDown()
{
	m_value =m_value+(m_itemsOnScreen-1.01);
	if ( m_value>m_maximum-m_itemsOnScreen)
	{
		m_value =m_maximum-m_itemsOnScreen;
	}
	if ( m_value <m_minimum)
	{
		m_value =m_minimum;
	}
	if ( m_maximum-m_minimum+1<m_itemsOnScreen)
	{
		m_value =m_minimum;
	}
	return true;
}

/*============================================================================*/
///
/// @brief Start to drag
///
/// @param p [in] Position to start.
///
/*============================================================================*/
bool Cslider::onDragStart( Cpoint p)
{
#ifndef TOUCH_SLIDER
	int n=m_image_y;
	if ( p.y<8*(n-1))
	{
		m_offDisabled =true;
		wheelUp();
		return true;
	}
	if ( p.y>8*(n+7))
	{
		m_offDisabled =true;
		wheelDown();
		return true;
	}
#endif
	return onDrag(p);
}

/*============================================================================*/
///
/// @brief Drag finger to object.
///
/// @param p [in] Position finger.
///
/*============================================================================*/
bool Cslider::onDragEnd( Cpoint p)
{
	onDrag( p);
	m_offDisabled =false;
	return true;
}

/*============================================================================*/
///
/// @brief Release the finger.
///
/// @param p [in] Last position finger.
///
/*============================================================================*/
bool Cslider::onDrag( Cpoint p)
{
	if ( m_offDisabled==true)
	{
		return false;
	}
	if ( m_sliderType ==SLIDER_VERTICAL)
	{
		int y=p.y-m_rect.top()*8-24;
		double value =m_minimum+(m_maximum-m_minimum-m_itemsOnScreen)*y/(m_rect.height()*8-48.0);
		if ( value>m_maximum-m_itemsOnScreen)
		{
			value =m_maximum-m_itemsOnScreen;
		}
		if ( m_maximum-m_minimum+1<m_itemsOnScreen)
		{
			value=m_minimum;
		}
		if ( value<m_minimum)
		{
			value =m_minimum;
		}
		if ( m_value !=value)
		{
			Log.write("Value slider =%d", (int)value);
			m_value =value;
			return true;
		}
	}
	return false;
}

bool Cslider::setRange( double minimum, double maximum)
{
	bool retVal =false;
	m_minimum =minimum;
	m_maximum =maximum;
	if ( m_value<m_minimum)
	{
		m_value =m_minimum;
		retVal =true;
	}
	if ( m_value>m_maximum-m_itemsOnScreen)
	{
		m_value =m_maximum-m_itemsOnScreen;
		retVal =true;
	}
	if ( m_value<m_minimum)
	{
		m_value =m_minimum;
		retVal =true;
	}
	return retVal;
}
