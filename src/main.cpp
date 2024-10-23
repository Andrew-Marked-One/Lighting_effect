#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cmath>

class Shape {
public:
    std::vector<sf::Vector2f> corners;
    sf::VertexArray m_shape;
    Shape() 
        : m_shape(sf::LineStrip) {}
    Shape& add(float x, float y) {
        m_shape.append(sf::Vector2f(x, y));
        corners.push_back(sf::Vector2f(x, y));
        return *this;
    }
};
struct Intercect {
    bool result;
    sf::Vector2f pos;
    Intercect(bool intersect, sf::Vector2f point)
        : result(intersect)
        , pos(point) {}
};

Intercect lineIntersect(sf::Vector2f& a, sf::Vector2f& b, sf::Vector2f& c, sf::Vector2f& d) {
    sf::Vector2f r = b - a;
    sf::Vector2f s = d - c;
    float rxs = r.x * s.y - r.y * s.x;
    sf::Vector2f cma = c - a;
    float t = (cma.x * s.y - cma.y * s.x) / rxs;
    float u = (cma.x * r.y - cma.y * r.x) / rxs;
    if (t >= 0 && u >= 0 && u <= 1) {
        return Intercect(true, sf::Vector2f(a.x + r.x * t, a.y + r.y * t));
    }
    else {
        return Intercect(false, sf::Vector2f(0, 0));
    }
}

int main(){
    float windowX = 1280.f;
    float windowY = 720.f;
    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "Line intersection!");
    window.setFramerateLimit(60);
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<sf::VertexArray> lines;
    sf::VertexArray triangleFan(sf::TriangleFan);

    shapes.push_back(std::make_shared<Shape>());
    shapes[0]->add(100.f, 100.f).add(200.f, 100.f).add(200.f, 200.f).add(100.f, 200.f).add(100.f, 100.f);
    shapes.push_back(std::make_shared<Shape>());
    shapes[1]->add(500.f, 500.f).add(700.f, 400.f).add(600.f, 650.f).add(500.f, 650.f).add(500.f, 500.f);
    shapes.push_back(std::make_shared<Shape>());
    shapes[2]->add(1000.f, 50.f).add(1025.f, 25.f).add(1050.f, 50.f).add(1000.f, 50.f);
    shapes.push_back(std::make_shared<Shape>());
    shapes[3]->add(1040.f, 410.f).add(1215.f, 514.f).add(1160.f, 626.f).add(980.f, 527.f).add(1040.f, 410.f);
    shapes.push_back(std::make_shared<Shape>());
    shapes[4]->add(551.f, 149.f).add(763.f, 253.f).add(570.f, 394.f).add(317.f, 466.f).add(125.f, 314.f).add(281.f, 339.f).add(284.f, 135.f).add(427.f, 255.f).add(551.f, 149.f);
    shapes.push_back(std::make_shared<Shape>());
    shapes[5]->add(0.f, 0.f).add(windowX, 0.f).add(windowX, windowY).add(0.f, windowY).add(0.f, 0.f);

    float lightbulbRadius = 9;
    sf::CircleShape lightbulb(lightbulbRadius);
    lightbulb.setOrigin(lightbulbRadius, lightbulbRadius);
    lightbulb.setFillColor(sf::Color::Red);

    float offsetAgnle = 0.00001f;
    int backgroundColor = 120;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
                //std::cout << mousePos.x << " " << mousePos.y << '\n';
                lightbulb.setPosition(mousePos);

                //add lines from the lightBulb to the corners
                lines.clear();
                sf::VertexArray line(sf::LineStrip);
                line.append(lightbulb.getPosition());
                line.append(lightbulb.getPosition());
                float scale = std::max(window.getSize().x, window.getSize().y);
                for (auto& shape : shapes) {
                    for (size_t i = 0; i < shape->corners.size() - 1; i++) {
                        line[1] = shape->corners[i];
                        lines.push_back(line);

                        sf::Vector2f dist = { line[1].position - line[0].position };
                        float angle = atan2(dist.y, dist.x);

                        sf::Vector2f offsetVal1 = { static_cast<float>(cos(angle + offsetAgnle)), static_cast<float>(sin(angle + offsetAgnle)) };
                        sf::Vector2f offsetVal2 = { static_cast<float>(cos(angle - offsetAgnle)), static_cast<float>(sin(angle - offsetAgnle)) };
                       
                        line[1].position = line[0].position + offsetVal1 * scale;
                        lines.push_back(line);
                        line[1].position = line[0].position + offsetVal2 * scale;
                        lines.push_back(line);
                    }
                }
                //check collisions with the vertecies
                for (auto& line : lines) {
                    for (auto& shape : shapes) {
                        for (size_t i = 0; i < shape->m_shape.getVertexCount() - 1; i++) {
                            Intercect result = lineIntersect(line[0].position, line[1].position, shape->m_shape[i].position, shape->m_shape[i + 1].position);
                            sf::Vector2f distPrev = { line[1].position - line[0].position };
                            sf::Vector2f distNew = { result.pos - line[0].position };
                            float distPrevSq = distPrev.x * distPrev.x + distPrev.y * distPrev.y;
                            float distNewSq = distNew.x * distNew.x + distNew.y * distNew.y;
                            if (result.result && distNewSq < distPrevSq) {
                                line[1].position = result.pos;
                            }
                        }
                    }
                }
                //sorting lines by angle
                std::sort(lines.begin(), lines.end(), [](const sf::VertexArray& a, const sf::VertexArray& b) {
                    float angle1 = atan2(a[1].position.y - a[0].position.y, a[1].position.x - a[0].position.x);
                    float angle2 = atan2(b[1].position.y - b[0].position.y, b[1].position.x - b[0].position.x);
                    return angle1 > angle2;
                });
                //creating trianglefan
                triangleFan.setPrimitiveType(sf::TriangleFan);
                triangleFan.resize(lines.size() + 1);
                triangleFan[0].position = lightbulb.getPosition();
                triangleFan[0].color = sf::Color(245, 245, 103, 150);
               
                for (int i = 1; i < lines.size(); ++i) {
                    triangleFan[i].position = lines[i][1].position;
                    triangleFan[i].color = sf::Color(245, 245, 103, 150);
                }
                triangleFan[triangleFan.getVertexCount() - 1].position = lines[0][1].position;
                triangleFan[triangleFan.getVertexCount() - 1].color = sf::Color(245, 245, 103, 150);
            }
        }



        window.clear(sf::Color(backgroundColor, backgroundColor, backgroundColor));
        for (auto& shape : shapes) {
            window.draw(shape->m_shape);
        }
        //for (auto& line : lines) {
            //window.draw(line);
        //}
        window.draw(triangleFan);
        window.draw(lightbulb);
        window.display();
    }
    return 0;
}
//copy