/*
 *  remote_control_widget.h - widget containing a VNC-view and controls for it
 *
 *  Copyright (c) 2006-2007 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
 *  
 *  This file is part of iTALC - http://italc.sourceforge.net
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */


#ifndef _REMOTE_CONTROL_WIDGET_H
#define _REMOTE_CONTROL_WIDGET_H


#include <QtCore/QTime>
#include <QtGui/QWidget>

#include "fast_qimage.h"


class vncView;
class remoteControlWidget;



class remoteControlWidgetToolBar : public QWidget
{
	Q_OBJECT
public:
	remoteControlWidgetToolBar( remoteControlWidget * _parent );
	virtual ~remoteControlWidgetToolBar();


public slots:
	void appear( void );
	void disappear( void );


protected:
	virtual void leaveEvent( QEvent * _e )
	{
		disappear();
		QWidget::leaveEvent( _e );
	}

/*	virtual void mousePressEvent( QMouseEvent * _e )
	{
		disappear();
		QWidget::mousePressEvent( _e );
	}*/

	virtual void paintEvent( QPaintEvent * _pe );


private slots:
	void updatePosition( void );
	void startConnection( void );
	void connectionEstablished( void );


private:
	remoteControlWidget * m_parent;
	bool m_disappear;
	bool m_connecting;
	QImage m_icon;
	fastQImage m_iconGray;
	QTime m_iconState;

} ;





class remoteControlWidget : public QWidget
{
	Q_OBJECT
public:
	remoteControlWidget( const QString & _host, bool _view_only = FALSE );
	virtual ~remoteControlWidget();

	QString host( void ) const;


public slots:
	void toggleFullScreen( bool );


protected:
	virtual void resizeEvent( QResizeEvent * );


private slots:
	void checkKeyEvent( Q_UINT32, bool );
	void updateUser( void );


private:
	vncView * m_vncView;
	remoteControlWidgetToolBar * m_toolBar;

	QString m_user;
	friend class remoteControlWidgetToolBar;

} ;


#endif

