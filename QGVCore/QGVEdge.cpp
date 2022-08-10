/***************************************************************
QGVCore
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/
#include <QGVEdge.h>
#include <QGVCore.h>
#include <QGVScene.h>
#include <QGVGraphPrivate.h>
#include <QGVEdgePrivate.h>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>

QGVEdge::QGVEdge(QGVEdgePrivate *edge, QGVScene *scene)
    : _scene(scene)
    , _edge(edge)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QGVEdge::~QGVEdge()
{
    _scene->removeItem(this);
    delete _edge;
}

QString QGVEdge::label() const
{
    return getAttribute("label");
}

QRectF QGVEdge::boundingRect() const
{
    return _path.boundingRect() | _head_arrow.boundingRect() | _tail_arrow.boundingRect() | _label_rect;
}

QPainterPath QGVEdge::shape() const
{
    QPainterPathStroker ps;
    ps.setCapStyle(_pen.capStyle());
    ps.setWidth(_pen.widthF() + 10);
    ps.setJoinStyle(_pen.joinStyle());
    ps.setMiterLimit(_pen.miterLimit());
    return ps.createStroke(_path);
}

void QGVEdge::setLabel(const QString &label)
{
    setAttribute("label", label);
}

void QGVEdge::setAttribute(const QString &name, const QString &value)
{
    char empty[] = "";
    agsafeset(_edge->edge(), name.toLocal8Bit().data(), value.toLocal8Bit().data(), empty);
}

QString QGVEdge::getAttribute(const QString &name) const
{
		char* value = agget(_edge->edge(), name.toLocal8Bit().data());
    if(value)
        return value;
    return QString();
}

void QGVEdge::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const bool isInvis = getAttribute("style").toLower() == "invis";

    if (labelItem_) labelItem_->setVisible(!isInvis);
    if (headLabelItem_) headLabelItem_->setVisible(!isInvis);
    if (tailLabelItem_) tailLabelItem_->setVisible(!isInvis);

    if (isInvis)
        return;

    painter->save();

    if(isSelected())
    {
        QPen tpen(_pen);
        tpen.setColor(_pen.color().darker(120));
        tpen.setStyle(Qt::DotLine);
        painter->setPen(tpen);
    }
    else
        painter->setPen(_pen);


    painter->drawPath(_path);

    painter->setBrush(QBrush(_pen.color(), Qt::SolidPattern));
    painter->drawPolygon(_head_arrow);
    painter->drawPolygon(_tail_arrow);

    painter->restore();
}

namespace
{
void update_label_item(QGraphicsTextItem *item, textlabel_t *label, qreal gheight)
{
    assert(item);

    if (label)
    {
        auto topt = item->document()->defaultTextOption();
        topt.setAlignment(Qt::AlignCenter);
        item->document()->setDefaultTextOption(topt);
        item->setHtml(label->text);
        item->adjustSize();

        item->setPos(0, 0);
        auto itemRect = item->boundingRect();
        auto labelCenter = QGVCore::toPoint(label->pos, gheight);
        itemRect.moveCenter(labelCenter);
        item->setPos(itemRect.topLeft());
        item->show();
    }
    else
        item->hide();
}
}

void QGVEdge::updateLayout()
{
    prepareGeometryChange();

    qreal gheight = QGVCore::graphHeight(_scene->_graph->graph());

    const splines* spl = ED_spl(_edge->edge());
    _path = QGVCore::toPath(spl, gheight);


    //Edge arrows
    if((spl->list != 0) && (spl->list->size%3 == 1))
    {
        if(spl->list->sflag)
        {
            _tail_arrow = toArrow(QLineF(QGVCore::toPoint(spl->list->list[0], gheight), QGVCore::toPoint(spl->list->sp, gheight)));
        }

        if(spl->list->eflag)
        {
            _head_arrow = toArrow(QLineF(QGVCore::toPoint(spl->list->list[spl->list->size-1], gheight), QGVCore::toPoint(spl->list->ep, gheight)));
        }
    }

    _pen.setWidth(1);
    _pen.setColor(QGVCore::toColor(getAttribute("color")));
    _pen.setStyle(QGVCore::toPenStyle(getAttribute("style")));

    // Edge label handling
    auto label_update_helper = [this, gheight] (QGraphicsTextItem **labelItemP, textlabel_t *label)
    {
        assert(labelItemP);

        if (label)
        {
            if (!*labelItemP)
                *labelItemP = new QGraphicsTextItem(this);

            update_label_item(*labelItemP, label, gheight);
        }
        else if (*labelItemP)
            (*labelItemP)->hide();
    };

    textlabel_t *label = ED_label(_edge->edge());

    if (!label)
       label = ED_xlabel(_edge->edge());

    label_update_helper(&labelItem_, label);

    label_update_helper(&headLabelItem_, ED_head_label(_edge->edge()));
    label_update_helper(&tailLabelItem_, ED_tail_label(_edge->edge()));

    setToolTip(getAttribute("tooltip"));
}

QPolygonF QGVEdge::toArrow(const QLineF &line) const
{
    QLineF n = line.normalVector();
    QPointF o(n.dx() / 3.0, n.dy() / 3.0);

    //Only support normal arrow type
    QPolygonF polygon;
    polygon.append(line.p1() + o);
    polygon.append(line.p2());
    polygon.append(line.p1() - o);

    return polygon;
}
