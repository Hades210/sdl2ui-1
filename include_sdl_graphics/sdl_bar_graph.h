/*============================================================================*/
/**  @file      sdl_bar_graph.h
 **  @ingroup   user_interface
 **  @brief		Draw bar.
 **
 **  Show a vertical bar with info.
 **
 **  @author     mensfort
 **
 **  @par Classes:
 **              CbarGraph
 */
/*------------------------------------------------------------------------------
 **  Copyright (c) Bart Houkes, 24 aug 2011
 **
 **  Copyright notice:
 **  This software is property of Bart Houkes.
 **  Unauthorized duplication and disclosure to third parties is forbidden.
 **============================================================================*/

#pragma once

/*------------- Standard includes --------------------------------------------*/
#include <string>
#include "sdl_graphics.h"
#include "sdl_dialog_object.h"

/// @brief  Forward declaration.
class Cdialog;

/// @brief  Create and display buttons.
class CbarGraph : public CdialogObject
{
public:
	CbarGraph( Cdialog *parent, double value1, bool stacked, const std::string &text,
			   int spacing, int helplines, colour back);
	CbarGraph( Cdialog *parent, double value1, double value2, bool stacked,
			   const std::string &text, int spacing, int helplines, colour back);
	virtual ~CbarGraph();

public:
	void    onPaint( int touch) { (void)touch; }
	void	onPaint( const Crect &rect, int touch);
	void	setColours( int n, colour background);
	void 	setRange( double minimum, double maximum);
	void 	rotate();
	void setColourHelpLines( colour n);

private:
	std::vector<colour> m_colour; ///< Colour bar.

	std::vector<double> m_value; ///< Current y-value.
	double 				m_minimum; ///< Minimum slider value.
	double  			m_maximum; ///< Maximum slider value.
	bool				m_stacked;
	int					m_spacing; ///< Left + right spacing together.
	colour 				m_background; ///< Background of graph.
	std::string			m_text; ///< Text below graph.
	int					m_textHeight; ///< Height of the text.
	bool				m_rotate; ///< Rotate the text.
	colour				m_helpLines; ///< To show the level.

public:
	static colour		m_defaultColour[8]; ///< Default colours
};
