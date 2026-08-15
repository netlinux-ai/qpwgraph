// qpwgraph_zone.cpp
//
/****************************************************************************
   Copyright (C) 2021-2025, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "qpwgraph_zone.h"
#include "qpwgraph_node.h"

#include <QGraphicsLinearLayout>
#include <QStyleOptionGraphicsItem>

#include <QPainter>
#include <QPen>
#include <QFont>


//----------------------------------------------------------------------------
// qpwgraph_zone -- Packed zone container.

qpwgraph_zone::qpwgraph_zone ( Qt::Orientation orientation,
	const QString& label, const QColor& color, QGraphicsItem *parent )
	: QGraphicsWidget(parent), m_label(label), m_color(color)
{
	m_layout = new QGraphicsLinearLayout(orientation);
	// Leave room at the top for the zone label; a little breathing
	// room everywhere else.
	m_layout->setContentsMargins(10.0, 22.0, 10.0, 10.0);
	m_layout->setSpacing(10.0);

	QGraphicsWidget::setLayout(m_layout);
	QGraphicsWidget::setAcceptedMouseButtons(Qt::NoButton);
	QGraphicsWidget::setZValue(-500.0);
}


// Membership.
//
// setParentItem() is explicit rather than relied-upon-implicitly: it's
// what keeps this node a QGraphicsItem child of the zone (so it paints
// on top of the zone's background, and so cable endpoints re-resolve
// correctly via qpwgraph_port's existing ItemScenePositionHasChanged
// handling) independently of whatever the layout itself may or may not
// do to the item's parentage.
//
// setParentItem() does *not* preserve scenePos() on its own -- pos()
// keeps its numeric value but that value is now relative to a
// different origin, so the item silently jumps unless we compensate.
// The jump after addNode() is invisible in practice (the layout's
// subsequent setGeometry() overwrites the position before anything
// repaints), but after removeNode() the item can sit at that wrong
// position indefinitely (mid-drag, or released back to freeform) --
// so removeNode() is where this actually matters.
void qpwgraph_zone::addNode ( qpwgraph_node *node, int index )
{
	const QPointF scene_pos = node->scenePos();
	node->setParentItem(this);
	node->setPos(mapFromScene(scene_pos));

	if (index < 0 || index >= m_layout->count())
		m_layout->addItem(node);
	else
		m_layout->insertItem(index, node);
}


void qpwgraph_zone::removeNode ( qpwgraph_node *node )
{
	const QPointF scene_pos = node->scenePos();
	m_layout->removeItem(node);
	node->setParentItem(nullptr);
	node->setPos(scene_pos);
}


bool qpwgraph_zone::containsNode ( qpwgraph_node *node ) const
{
	const int n = m_layout->count();
	for (int i = 0; i < n; ++i) {
		if (m_layout->itemAt(i) == static_cast<QGraphicsLayoutItem *> (node))
			return true;
	}
	return false;
}


QList<qpwgraph_node *> qpwgraph_zone::takeAllNodes (void)
{
	QList<qpwgraph_node *> nodes;

	while (m_layout->count() > 0) {
		qpwgraph_node *node
			= static_cast<qpwgraph_node *> (m_layout->itemAt(0));
		removeNode(node);
		nodes.append(node);
	}

	return nodes;
}


int qpwgraph_zone::count (void) const
{
	return m_layout->count();
}


// Index that a drop at pos (this item's local coordinates) would land at.
int qpwgraph_zone::indexForDropPoint ( const QPointF& pos ) const
{
	const bool horiz = (m_layout->orientation() == Qt::Horizontal);
	const qreal p = horiz ? pos.x() : pos.y();

	const int n = m_layout->count();
	for (int i = 0; i < n; ++i) {
		const QRectF& rect = m_layout->itemAt(i)->geometry();
		const qreal mid = horiz
			? rect.left() + 0.5 * rect.width()
			: rect.top()  + 0.5 * rect.height();
		if (p < mid)
			return i;
	}

	return n;
}


void qpwgraph_zone::paint ( QPainter *painter,
	const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ )
{
	QColor fill(m_color);
	fill.setAlpha(24);
	QColor line(m_color);
	line.setAlpha(170);

	painter->setPen(QPen(line, 1.2, Qt::DashLine));
	painter->setBrush(fill);
	painter->drawRoundedRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5), 6.0, 6.0);

	QFont font = painter->font();
	font.setBold(true);
	painter->setFont(font);
	painter->setPen(line);
	painter->drawText(boundingRect().adjusted(8.0, 4.0, -8.0, -4.0),
		Qt::AlignLeft | Qt::AlignTop, m_label);
}


// end of qpwgraph_zone.cpp
