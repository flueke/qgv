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
#include "QGVScene.h"

#include <cstdio>
#include <iostream>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QGVCore.h>
#include <QGVEdge.h>
#include <QGVEdgePrivate.h>
#include <QGVGraphPrivate.h>
#include <QGVGvcPrivate.h>
#include <QGVNode.h>
#include <QGVNodePrivate.h>
#include <QGVSubGraph.h>
#include <QPainter>
#include <QVarLengthArray>

QGVScene::QGVScene(QObject *parent)
    : QGVScene("g", parent)
{
}

QGVScene::QGVScene(const QString &name, QObject *parent) : QGraphicsScene(parent)
{
    _context = new QGVGvcPrivate(gvContext());
    _graph = new QGVGraphPrivate(agopen(name.toLocal8Bit().data(), Agdirected, NULL));
    //setGraphAttribute("fontname", QFont().family());
}

QGVScene::~QGVScene()
{
    gvFreeLayout(_context->context(), _graph->graph());
    agclose(_graph->graph());
    gvFreeContext(_context->context());
    delete _graph;
    delete _context;
}

void QGVScene::setGraphAttribute(const QString &name, const QString &value)
{
    agattr(_graph->graph(), AGRAPH, name.toLocal8Bit().data(), value.toLocal8Bit().data());
}

void QGVScene::setNodeAttribute(const QString &name, const QString &value)
{
    agattr(_graph->graph(), AGNODE, name.toLocal8Bit().data(), value.toLocal8Bit().data());
}

void QGVScene::setEdgeAttribute(const QString &name, const QString &value)
{
    agattr(_graph->graph(), AGEDGE, name.toLocal8Bit().data(), value.toLocal8Bit().data());
}

QGVNode *QGVScene::addNode(const QString &label, const QString &id)
{
    Agnode_t *node{};

    if (!id.isEmpty())
        node = agnode(_graph->graph(), id.toLocal8Bit().data(), true);
    else
        node = agnode(_graph->graph(), NULL, TRUE);

    if(node == NULL)
    {
        qWarning()<<"Invalid node :"<<label;
        return 0;
    }
    QGVNode *item = new QGVNode(new QGVNodePrivate(node, _graph->graph()), this);
    item->setLabel(label);
    addItem(item);
    _nodes.append(item);
    return item;
}

QGVEdge *QGVScene::addEdge(QGVNode *source, QGVNode *target, const QString &label)
{
    Agedge_t* edge = agedge(_graph->graph(), source->_node->node(), target->_node->node(), NULL, TRUE);
    if(edge == NULL)
    {
        qWarning()<<"Invalid egde :"<<label;
        return 0;
    }

    QGVEdge *item = new QGVEdge(new QGVEdgePrivate(edge), this);
    item->setLabel(label);
    addItem(item);
    _edges.append(item);
    return item;
}

QGVSubGraph *QGVScene::addSubGraph(const QString &name, bool cluster)
{
    Agraph_t* sgraph;
    if(cluster)
        sgraph = agsubg(_graph->graph(), ("cluster_" + name).toLocal8Bit().data(), TRUE);
    else
        sgraph = agsubg(_graph->graph(), name.toLocal8Bit().data(), TRUE);

    if(sgraph == NULL)
    {
        qWarning()<<"Invalid subGraph :"<<name;
        return 0;
    }

    QGVSubGraph *item = new QGVSubGraph(new QGVGraphPrivate(sgraph), this);
    addItem(item);
    _subGraphs.append(item);
    return item;
}

void QGVScene::deleteNode(QGVNode* node)
{
    QList<QGVNode *>::iterator it = std::find(_nodes.begin(), _nodes.end(), node);
    if(it == _nodes.end())
    {
        std::cout << "Error, node not part of Scene" << std::endl;
        return;
    }
    std::cout << "delNode ret " << agdelnode(node->_node->graph(), node->_node->node()) << std::endl;;
    _nodes.erase(it);
    delete node;
}

void QGVScene::deleteEdge(QGVEdge* edge)
{
    std::cout << "delEdge ret " << agdeledge(_graph->graph(), edge->_edge->edge()) << std::endl;
    QList<QGVEdge *>::iterator it = std::find(_edges.begin(), _edges.end(), edge);
    if(it == _edges.end())
    {
        std::cout << "Error, QGVEdge not part of Scene" << std::endl;
        return;
    }
    _edges.erase(it);
    delete edge;
}

void QGVScene::deleteSubGraph(QGVSubGraph *subgraph)
{
    std::cout << "Removing sug " << subgraph->_sgraph->graph() << std::endl;
    std::cout << "delSubg ret " << agclose(subgraph->_sgraph->graph()) << std::endl;
    QList<QGVSubGraph *>::iterator it = std::find(_subGraphs.begin(), _subGraphs.end(), subgraph);
    if(it == _subGraphs.end())
    {
        std::cout << "Error, QGVSubGraph not part of Scene" << std::endl;
        return;
    }
    _subGraphs.erase(it);

    delete subgraph;
}

void QGVScene::setRootNode(QGVNode *node)
{
    Q_ASSERT(_nodes.contains(node));
    char root[] = "root";
    agset(_graph->graph(), root, node->label().toLocal8Bit().data());
}

Agraph_t *QGVScene::graph()
{
    return _graph->graph();
}

QString QGVScene::toDot() const
{
    if (auto outfile = std::tmpfile())
    {
        agwrite(const_cast<QGVScene *>(this)->graph(), outfile);
        auto bytesWritten = std::ftell(outfile);
        std::rewind(outfile);
        QByteArray data(bytesWritten, '\0');
        std::fread(data.data(), bytesWritten, 1, outfile);
        std::fclose(outfile);
        return QString::fromLocal8Bit(data);
    }
    return {};
}

void QGVScene::newGraph(const QString &name)
{
    clearGraphItems();
    agclose(_graph->graph());
    _graph->setGraph(agopen(name.toLocal8Bit().data(), Agdirected, NULL));
}

void QGVScene::loadLayout(const QString &text)
{
    clearGraphItems();
    agclose(_graph->graph());

    _graph->setGraph(QGVCore::agmemread2(text.toLocal8Bit().constData()));

    if (!_graph->graph())
        return;

    //Debug output
		//gvRenderFilename(_context->context(), _graph->graph(), "png", "debug.png");

    // Add subgraphs first to layer them below other items.
    // Note: The loop only picks up the immediate subgraphs of the given graph.
    // Recursion would be needed to find all subgraphs.
    for (auto sg = agfstsubg(_graph->graph()); sg; sg = agnxtsubg(sg))
    {
        auto sgItem = new QGVSubGraph(new QGVGraphPrivate(sg), this);
        addItem(sgItem);
        _subGraphs.append(sgItem);
    }

    //Read nodes and edges
    for (Agnode_t* node = agfstnode(_graph->graph()); node != NULL; node = agnxtnode(_graph->graph(), node))
    {
        QGVNode *inode = new QGVNode(new QGVNodePrivate(node, _graph->graph()), this);
        //inode->updateLayout();
        addItem(inode);
        _nodes.append(inode);
        for (Agedge_t* edge = agfstout(_graph->graph(), node); edge != NULL; edge = agnxtout(_graph->graph(), edge))
        {
            QGVEdge *iedge = new QGVEdge(new QGVEdgePrivate(edge), this);
            iedge->setFlag(QGraphicsItem::ItemIsSelectable, false);
            //iedge->updateLayout();
            addItem(iedge);
            _edges.append(iedge);
        }

    }

    applyLayout();
}

void QGVScene::applyLayout()
{
    if(gvLayout(_context->context(), _graph->graph(), "dot") != 0)
    {
        /*
         * Si plantage ici :
         *  - Verifier que les dll sont dans le repertoire d'execution
         *  - Verifie que le fichier "configN" est dans le repertoire d'execution !
         */
        qCritical()<<"Layout render error"<<agerrors()<<QString::fromLocal8Bit(aglasterr());
        return;
    }

    //Debug output
		//gvRenderFilename(_context->context(), _graph->graph(), "canon", "debug.dot");
		//gvRenderFilename(_context->context(), _graph->graph(), "png", "debug.png");

    updateLayout();

    gvFreeLayout(_context->context(), _graph->graph());

    setSceneRect(itemsBoundingRect());
    update();
}

void QGVScene::clearGraphItems()
{
    gvFreeLayout(_context->context(), _graph->graph());
    for (auto node: _nodes)
        delete node;
    _nodes.clear();

    for (auto edge: _edges)
        delete edge;
    _edges.clear();

    for (auto sg: _subGraphs)
        delete sg;
    _subGraphs.clear();

    if (_graphLabelItem)
    {
        removeItem(_graphLabelItem);
        delete _graphLabelItem;
        _graphLabelItem = nullptr;
    }
}

void QGVScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    QGraphicsItem *item = itemAt(contextMenuEvent->scenePos(), QTransform());
    if(item)
    {
        item->setSelected(true);
        if(item->type() == QGVNode::Type)
            emit nodeContextMenu(qgraphicsitem_cast<QGVNode*>(item));
        else if(item->type() == QGVEdge::Type)
            emit edgeContextMenu(qgraphicsitem_cast<QGVEdge*>(item));
        else if(item->type() == QGVSubGraph::Type)
            emit subGraphContextMenu(qgraphicsitem_cast<QGVSubGraph*>(item));
        else
            emit graphContextMenuEvent();
    }
    QGraphicsScene::contextMenuEvent(contextMenuEvent);
}

void QGVScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    auto items = this->items(mouseEvent->scenePos());

    for (auto item: items)
    {
        if(item->type() == QGVNode::Type)
        {
            emit nodeDoubleClick(qgraphicsitem_cast<QGVNode*>(item));
            break;
        }
        else if(item->type() == QGVEdge::Type)
        {
            emit edgeDoubleClick(qgraphicsitem_cast<QGVEdge*>(item));
            break;
        }
        else if(item->type() == QGVSubGraph::Type)
        {
            emit subGraphDoubleClick(qgraphicsitem_cast<QGVSubGraph*>(item));
            break;
        }
    }

    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void QGVScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    if (!shouldDrawBackgroundGrid())
        return;

    const int gridSize = 25;

    const qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    const qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->setPen(QColor(Qt::lightGray).lighter(110));
    painter->drawLines(lines.data(), lines.size());
    painter->setPen(Qt::black);
    //painter->drawRect(sceneRect());
}

void QGVScene::updateLayout()
{
    for (auto n: _nodes)
        n->updateLayout();

    for (auto e: _edges)
        e->updateLayout();

    for (auto s: _subGraphs)
        s->updateLayout();

    //Graph label
    if (textlabel_t *xlabel = GD_label(_graph->graph()))
    {
        if (!_graphLabelItem)
        {
            _graphLabelItem = new QGraphicsTextItem;
            addItem(_graphLabelItem);
        }

        _graphLabelItem->setPos(QGVCore::centerToOrigin(QGVCore::toPoint(xlabel->pos, QGVCore::graphHeight(_graph->graph())), xlabel->dimen.x, -4));
        _graphLabelItem->setPlainText(xlabel->text);
        _graphLabelItem->show();
    }
    else if (_graphLabelItem)
        _graphLabelItem->hide();
}
