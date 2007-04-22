/*
 * tool_button.cpp - implementation of iTALC-tool-button
 *
 * Copyright (c) 2006-2007 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of iTALC - http://italc.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */
 

#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>

#include "tool_button.h"
#include "fast_qimage.h"



const int MARGIN = 10;
const int ROUNDED = 2000;

bool toolButton::s_toolTipsDisabled = FALSE;


toolButton::toolButton( const QPixmap & _pixmap, const QString & _label,
				const QString & _title, const QString & _desc,
				QObject * _receiver, const char * _slot,
							QWidget * _parent ) :
	QToolButton( _parent ),
	m_pixmap( _pixmap ),
	m_img( fastQImage( _pixmap.toImage() ).scaled( 32, 32 ) ),
	m_colorizeLevel( 0 ),
	m_fadeBack( FALSE ),
	m_label( _label ),
	m_title( _title ),
	m_descr( _desc )
{
	setAttribute( Qt::WA_NoSystemBackground, true );
	QFont f = font();
	f.setPointSize( 7 );
	setFont( f );
	//setFixedWidth( qMax<int>( m_img.width(), fontMetrics().width( m_hintLabel ) )+20 );
	//setFixedHeight( 48 );
	setFixedSize( 88, 48 );

	setText( m_title );
	if( _receiver != NULL && _slot != NULL )
	{
		connect( this, SIGNAL( clicked() ), _receiver, _slot );
	}

	// set mask
	QBitmap b( size() );
	b.clear();

	QPainter p( &b );
	p.setBrush( Qt::color1 );
	p.setPen( Qt::color1 );
	p.drawRoundRect( 0, 0, width() - 1, height() - 1,
					1000 / width(), 1000 / height() );
	setMask( b );
}




toolButton::~toolButton()
{
}




void toolButton::enterEvent( QEvent * _e )
{
	m_fadeBack = FALSE;
	if( m_colorizeLevel == 0 )
	{
		updateColorLevel();
	}
	if( !s_toolTipsDisabled && !m_title.isEmpty() && !m_descr.isEmpty() )
	{
		QPoint p = mapToGlobal( QPoint( 0, 0 ) );
		int scr = QApplication::desktop()->isVirtualDesktop() ?
				QApplication::desktop()->screenNumber( p ) :
				QApplication::desktop()->screenNumber( this );

#ifdef Q_WS_MAC
		QRect screen = QApplication::desktop()->availableGeometry(
									scr );
#else
		QRect screen = QApplication::desktop()->screenGeometry( scr );
#endif

		toolButtonTip * tbt = new toolButtonTip( m_pixmap, m_title,
							m_descr,
				QApplication::desktop()->screen( scr ), this );
		connect( this, SIGNAL( mouseLeftButton() ),
							tbt, SLOT( close() ) );

		if( p.x() + tbt->width() > screen.x() + screen.width() )
			p.rx() -= 4;// + tbt->width();
		if( p.y() + tbt->height() > screen.y() + screen.height() )
			p.ry() -= 30 + tbt->height();
		if( p.y() < screen.y() )
			p.setY( screen.y() );
		if( p.x() + tbt->width() > screen.x() + screen.width() )
			p.setX( screen.x() + screen.width() - tbt->width() );
		if( p.x() < screen.x() )
			p.setX( screen.x() );
		if( p.y() + tbt->height() > screen.y() + screen.height() )
			p.setY( screen.y() + screen.height() - tbt->height() );
		tbt->move( p += QPoint( -4, 46 ) );
		tbt->show();
	}
	QToolButton::enterEvent( _e );
}




void toolButton::leaveEvent( QEvent * _e )
{
	emit mouseLeftButton();
	m_fadeBack = TRUE;
	if( m_colorizeLevel == 255 )
	{
		updateColorLevel();
	}
	QToolButton::leaveEvent( _e );
}




void toolButton::mousePressEvent( QMouseEvent * _me )
{
	emit mouseLeftButton();
	QToolButton::mousePressEvent( _me );
}




void toolButton::paintEvent( QPaintEvent * _pe )
{
	const bool active = isDown() || isChecked();
	const QColor ctbl[2][4] = {
				{
					QColor( 64, 128, 255 ),
					QColor( 32, 64, 192 ),
					QColor( 16, 32, 128 ),
					QColor( 0, 16, 64 )
				},
				{
					QColor( 255, 224, 0, m_colorizeLevel ),
					QColor( 224, 192, 0, m_colorizeLevel ),
					QColor( 192, 160, 0, m_colorizeLevel ),
					QColor( 96, 48, 0, m_colorizeLevel )
				}
				} ;

	QPainter p( this );
	p.fillRect( rect(), Qt::black );
	p.setRenderHint( QPainter::Antialiasing );
	QLinearGradient lingrad( 0, 0, 0, height() );
	lingrad.setColorAt( 0, ctbl[0][0] );
	lingrad.setColorAt( 0.38, ctbl[0][1] );
	lingrad.setColorAt( 0.42, ctbl[0][2] );
	lingrad.setColorAt( 1, ctbl[0][3] );
	p.setBrush( lingrad );
	p.drawRoundRect( 1, 1, width()-2, height()-2, 1000 / width(),
							1000 / height() );

	if( m_colorizeLevel )
	{
		lingrad = QLinearGradient( 0, 0, 0, height() );
		lingrad.setColorAt( 0, ctbl[1][0] );
		lingrad.setColorAt( 0.38, ctbl[1][1] );
		lingrad.setColorAt( 0.42, ctbl[1][2] );
		lingrad.setColorAt( 1, ctbl[1][3] );
		p.setBrush( lingrad );
		p.drawRoundRect( 1, 1, width()-2, height()-2, 1000 / width(),
							1000 / height() );
	}
	p.setBrush( QBrush() );

	p.fillRect( rect(), QColor( 0, 0, 0, active ? 128 : 0 ) );

	QPen pen( QColor( 255, 255, 255, 128 ) );
	pen.setWidthF( 1.3f );
	p.setPen( pen );
	p.drawRoundRect( 0, 0, width()-1, height()-1, 1000 / width(),
							1000 / height() );
	QPen pen2 = pen;
	pen.setColor( QColor( 0, 0, 0, 128 ) );
	p.setPen( pen );
	p.drawRoundRect( 1, 1, width()-3, height()-3, 1000 / width(),
							1000 / height() );
	p.setPen( pen2 );
	p.drawRoundRect( 2, 2, width()-2, height()-2, 1000 / width(),
							1000 / height() );

	const int dd = isDown() ? 1 : 0;
	const QPoint pt = QPoint( (width() - m_img.width() ) / 2 + dd, 3 + dd);
	p.drawImage( pt, m_img );

	const int w = p.fontMetrics().width( m_label );
	p.setPen( Qt::black );
	p.drawText( ( width() -w ) / 2 + 1+dd, height() - 4+dd, m_label );
	p.setPen( Qt::white );
	p.drawText( ( width() - w ) / 2+dd, height() - 5+dd, m_label );

}





void toolButton::updateColorLevel( void )
{
	bool again;
	if( m_fadeBack )
	{
		m_colorizeLevel = qMax( 0, m_colorizeLevel - 10 );
		again = m_colorizeLevel > 0;
	}
	else
	{
		m_colorizeLevel = qMin( 255, m_colorizeLevel + 10 );
		again = m_colorizeLevel < 255;
	}
	update();
	if( again )
	{
		QTimer::singleShot( 10, this, SLOT( updateColorLevel() ) );
	}
}






toolButtonTip::toolButtonTip( const QPixmap & _pixmap, const QString & _title,
				const QString & _description,
				QWidget * _parent, QWidget * _tool_btn ) :
	QWidget( _parent, Qt::ToolTip ),
	m_icon( fastQImage( _pixmap ).scaled( 72, 72 ) ),
	m_title( _title ),
	m_description( _description ),
	m_dissolveSize( 24 ),
	m_toolButton( _tool_btn )
{
	setAttribute( Qt::WA_DeleteOnClose, TRUE );
	setAttribute( Qt::WA_NoSystemBackground, TRUE );

	resize( sizeHint() );
	updateMask();
}




QSize toolButtonTip::sizeHint( void ) const
{
	QFont f = font();
	f.setBold( TRUE );
	int title_w = QFontMetrics( f ).width( m_title );
	QRect desc_rect = fontMetrics().boundingRect( QRect( 0, 0, 250, 100 ),
					Qt::TextWordWrap, m_description );

	return QSize( MARGIN + m_icon.width() + MARGIN +
				qMax( title_w, desc_rect.width() ) + MARGIN,
			MARGIN + qMax( m_icon.height(), fontMetrics().height() +
						MARGIN + desc_rect.height() ) +
								MARGIN );
}




void toolButtonTip::paintEvent( QPaintEvent * _pe )
{
	QPainter p( this );
	p.drawImage( 0, 0, m_bg );
}




void toolButtonTip::resizeEvent( QResizeEvent * _re )
{
	const QColor color_frame = QColor( 48, 48, 48 );
	m_bg = QImage( size(), QImage::Format_ARGB32 );
	m_bg.fill( color_frame.rgba() );
	QPainter p( &m_bg );
	p.setRenderHint( QPainter::Antialiasing );
	QPen pen( color_frame );
	pen.setWidthF( 1.5 );
	p.setPen( pen );
	QLinearGradient grad( 0, 0, 0, height() );
	const QColor color_top = palette().color( QPalette::Active,
						QPalette::Window ).light( 120 );
	grad.setColorAt( 0, color_top );
	grad.setColorAt( 1, palette().color( QPalette::Active,
						QPalette::Window ).
							light( 80 ) );
	p.setBrush( grad );
	p.drawRoundRect( 0, 0, width() - 1, height() - 1,
					ROUNDED / width(), ROUNDED / height() );
	if( m_toolButton )
	{
		QPoint pt = m_toolButton->mapToGlobal( QPoint( 0, 0 ) );
		p.setPen( color_top );
		p.setBrush( color_top );
		p.setRenderHint( QPainter::Antialiasing, FALSE );
		p.drawLine( pt.x() - x(), 0,
				pt.x() + m_toolButton->width() - x() - 2, 0 );
		const int dx = pt.x() - x();
		p.setRenderHint( QPainter::Antialiasing, TRUE );
		if( dx < 10 && dx >= 0 )
		{
			p.setPen( pen );
			p.drawImage( dx+1, 0, m_bg.copy( 20, 0, 10-dx, 10 ) );
			p.drawImage( dx, 0, m_bg.copy( 0, 10, 1, 10-dx*2 ) );
		}
	}
	p.setPen( Qt::black );

	p.drawImage( MARGIN, MARGIN, m_icon );
	QFont f = p.font();
	f.setBold( TRUE );
	p.setFont( f );
	const int title_x = MARGIN + m_icon.width() + MARGIN;
	const int title_y = MARGIN + fontMetrics().height() - 2;
	p.drawText( title_x, title_y, m_title );

	f.setBold( FALSE );
	p.setFont( f );
	p.drawText( QRect( title_x, title_y + MARGIN,
					width() - MARGIN - title_x,
					height() - MARGIN - title_y ),
					Qt::TextWordWrap, m_description );

	updateMask();
	QWidget::resizeEvent( _re );
}




void toolButtonTip::updateMask( void )
{
	// as this widget has not a rectangular shape AND is a top
	// level widget (which doesn't allow painting only particular
	// regions), we have to set a mask for it
	QBitmap b( size() );
	b.clear();

	QPainter p( &b );
	p.setBrush( Qt::color1 );
	p.setPen( Qt::color1 );
	p.drawRoundRect( 0, 0, width() - 1, height() - 1,
					ROUNDED / width(), ROUNDED / height() );

	if( m_toolButton )
	{
		QPoint pt = m_toolButton->mapToGlobal( QPoint( 0, 0 ) );
		const int dx = pt.x()-x();
		if( dx < 10 && dx >= 0 )
		{
			p.fillRect( dx, 0, 10, 10, Qt::color1 );
		}
	}
/*	p.setBrush( Qt::color0 );
	p.setPen( Qt::color0 );

	if( m_dissolveSize > 0 )
	{
		const int size = 16;
		for( Q_UINT16 y = 0; y < height() + size; y += size )
		{
			Q_INT16 s = m_dissolveSize * width() / 128;
			for( Q_INT16 x = width(); x > -size; x -= size, s -= 2 )
			{
				if( s < 0 )
				{
					s = 0;
				}
				p.drawEllipse( x - s / 2, y - s / 2, s, s );
			}
		}
	}*/

	setMask( b );

/*	if( --m_dissolveSize >= 0 )
	{
		QTimer::singleShot( 20, this, SLOT( updateMask() ) );
	}*/
}




#include "tool_button.moc"
