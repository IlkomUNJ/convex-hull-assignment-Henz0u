#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QFont>

struct Point2D {
    int x, y;

    Point2D() : x(0), y(0) {}
    Point2D(int x, int y) : x(x), y(y) {}
    Point2D(const QPoint& qp) : x(qp.x()), y(qp.y()) {}

    QPoint toQPoint() const { return QPoint(x, y); }

    // Equality operator
    bool operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }

    // Inequality operator
    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }

    // Less than operator for sorting
    bool operator<(const Point2D& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

struct AlgorithmResult {
    QVector<Point2D> hull;
    int iterations;
    QString algorithmName;

    AlgorithmResult() : iterations(0) {}
};

class DrawingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void clearPoints();
    void runSlowConvexHull();
    void runFastConvexHull();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // Slow Convex Hull (O(n³) brute force) helper functions
    AlgorithmResult computeSlowConvexHull();
    bool isLeftOfLine(const Point2D& point, const Point2D& lineStart, const Point2D& lineEnd);
    bool isValidEdge(const Point2D& p, const Point2D& q, const QVector<Point2D>& points, int& iterations);
    void constructHullFromEdges(const QVector<QPair<Point2D, Point2D>>& edges, QVector<Point2D>& hull);

    // Fast Convex Hull (Andrew's monotone chain) helper functions
    AlgorithmResult computeFastConvexHull();
    int cross(const Point2D& O, const Point2D& A, const Point2D& B);

    void drawPoints(QPainter& painter);
    void drawConvexHull(QPainter& painter);
    void drawStatistics(QPainter& painter);

    QVector<Point2D> points;
    AlgorithmResult currentResult;
    bool showResult;
};

#endif // DRAWINGWIDGET_H
