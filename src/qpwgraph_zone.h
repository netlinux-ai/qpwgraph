// qpwgraph_zone.h
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

#ifndef __qpwgraph_zone_h
#define __qpwgraph_zone_h

#include <QGraphicsWidget>

#include <QColor>
#include <QString>
#include <QList>


// Forward decls.
class QGraphicsLinearLayout;
class QStyleOptionGraphicsItem;

class qpwgraph_node;


//----------------------------------------------------------------------------
// qpwgraph_zone -- A packed zone container (Inputs/Middle/Outputs/Unused).
//
// A real QGraphicsWidget owning a QGraphicsLinearLayout: member nodes are
// stacked without overlap because the layout engine positions them, not
// because we computed and assigned a position by convention. Reassigning a
// node to a different zone is a real removeNode()/addNode() pair between two
// qpwgraph_zone instances, not a copied coordinate.
//
// See qpwgraph-zones-proposal.html for the design this implements.

class qpwgraph_zone : public QGraphicsWidget
{
public:

	// Constructor.
	qpwgraph_zone(Qt::Orientation orientation,
		const QString& label, const QColor& color,
		QGraphicsItem *parent = nullptr);

	// Graphics item type.
	enum { Type = QGraphicsItem::UserType + 5 };

	int type() const override { return Type; }

	// Membership.
	void addNode(qpwgraph_node *node, int index = -1);
	void removeNode(qpwgraph_node *node);
	bool containsNode(qpwgraph_node *node) const;

	// Empties the zone, returning everything it held (in order). Used
	// when zoned layout is turned off, to release every node back to
	// the freeform canvas.
	QList<qpwgraph_node *> takeAllNodes();

	int count() const;

	// Index (in this item's own local coordinates) that a drop at pos
	// would land at, for drag-to-reorder/insert. Used by the canvas's
	// drag-and-drop handling.
	int indexForDropPoint(const QPointF& pos) const;

protected:

	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

	QGraphicsLinearLayout *m_layout;

	QString m_label;
	QColor  m_color;
};


#endif	// __qpwgraph_zone_h

// end of qpwgraph_zone.h
