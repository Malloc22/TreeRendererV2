//---------------------------

#ifndef TREERENDERER_HPP_INCLUDED
#define TREERENDERER_HPP_INCLUDED

//---------------------------

#include "included_libraries.hpp"

#include "Map.hpp"

//---------------------------

#define RADIUS 18
#define HEIGHT 100
#define WIDTH 5
#define LINE 30

//---------------------------

class TreeRenderer : public sf::Transformable, public sf::Drawable {
public:

    struct Item {

        Item* root = nullptr;

        sf::CircleShape shape;
        sf::Vertex line[2];

        sf::Text text;

        int key;
        int level,
            shift;

        int specFlag = 0;

        std::string data;

        void unfocused() { this->shape.setOutlineColor(sf::Color::Transparent); }
        void focused() { this->shape.setOutlineColor(sf::Color::Black); }
    };

private:

    typedef std::vector<TreeRenderer::Item*> items_t;

public:

    TreeRenderer();
    ~TreeRenderer();

    void init(sf::Font& font, sf::RenderWindow& window);
    void setTargetTree(Node<int, char>* root);

    void handleEvent(const sf::Event& event);
    void update(sf::Time elapsedTime);

    TreeRenderer::Item* getFocus() { return m_focusItem; }

    void clear();

private:

    items_t m_items;

    TreeRenderer::Item* m_focusItem;

    sf::Vector2f m_lastPosMouse;
    sf::Vector2f m_currentPosMouse;

    sf::View m_view;
    sf::Font m_font;

    sf::RenderWindow* m_window;

    sf::Time m_time;

    bool m_pressState;

    const sf::Color m_red,
                    m_gray;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    TreeRenderer::Item* testPoint(sf::Vector2f pos);
};

//---------------------------

#endif // TREERENDERER_HPP_INCLUDED

//---------------------------
