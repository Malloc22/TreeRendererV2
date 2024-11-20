//---------------------------

#include "TreeRenderer.hpp"

//---------------------------

TreeRenderer::TreeRenderer() : m_red({255, 102, 102}), m_gray({ 51, 51, 51 }) {

    m_window = nullptr;
    m_focusItem = nullptr;

    m_pressState = false;
}

//---------------------------

TreeRenderer::~TreeRenderer() {
    this->clear();
}

//---------------------------

void TreeRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    states.transform.combine(this->getTransform());

    for(const Item* item : m_items) {
        target.draw(item->line, 2, sf::Lines, states);
        target.draw(item->shape, states);
        target.draw(item->text);
    }
}

//---------------------------

TreeRenderer::Item* TreeRenderer::testPoint(sf::Vector2f pos) {
    for(TreeRenderer::Item* item : m_items) {
        if(item->shape.getGlobalBounds().contains(pos))
            return item;
    }

    return nullptr;
}

//---------------------------

void TreeRenderer::init(sf::Font& font, sf::RenderWindow& window) {
    m_font = font;
    m_window = &window;

    m_view.reset({0,0, 1280, 720});
    m_window->setView(m_view);
}

//---------------------------

void TreeRenderer::setTargetTree(Node<int, char>* root) {

    this->clear();

    if(!root || !m_window)
        return;

    std::queue<std::pair<Node<int, char>*, TreeRenderer::Item*>> q;
    q.push({ root, nullptr });

    int lvl = 0;
    int height = 0;

    TreeRenderer::Item* item = nullptr;

    while (!q.empty()) {
        std::pair<Node<int, char>*, TreeRenderer::Item*> current = q.front();
        q.pop();

        if(current.first == nullptr) {
            m_items.push_back(nullptr);

            if(current.second->specFlag == 0) {
                current.second->specFlag = 1;
                q.push({ nullptr, current.second });
                q.push({ nullptr, current.second });
            }

            /*if(current.second->specFlag == 1) {
                current.second->specFlag = 2;
                q.push({ nullptr, current.second });
            }*/

            continue;
        }

        if(height != current.first->height) {
            ++lvl;
            height = current.first->height;
        }

        item = new TreeRenderer::Item;

        item->root = current.second;
        item->level = lvl;
        item->data = current.first->data;
        item->key = current.first->key;

        m_items.push_back(item);

        q.push({ current.first->left, item });
        q.push({ current.first->right, item });
    }

    int maxlvl = root->height;
    int shift = 0;

    lvl = 0;

    for(Item* item : m_items) {

        if(shift & 1 << lvl) {
            lvl++;
            shift = 0;
            //std::cout << std::endl;
        }

        if(item) {
            item->shift = shift;
            item->shape.setRadius(RADIUS);
            item->shape.setOrigin(item->shape.getGlobalBounds().width / 2.0f,
                                  item->shape.getGlobalBounds().height / 2.0f);

            item->shape.setFillColor(lvl & 1 ? m_red : m_gray);
            item->shape.setOutlineColor(sf::Color::Transparent);
            item->shape.setOutlineThickness(-2.0f);

            item->text.setFont(m_font);
            item->text.setString(std::to_string(item->key) + "/" + item->data);
            item->text.setCharacterSize(RADIUS - 2);

            sf::FloatRect textRect = item->text.getLocalBounds();
            item->text.setOrigin(   (int)(textRect.left + textRect.width/2.0f),
                                    (int)(textRect.top  + textRect.height/2.0f));

            if(!item->root) {

                //std::cout << " " << item->key << " ";

                sf::Vector2f size = { (float)m_window->getSize().x, (float)m_window->getSize().y };
                item->shape.setPosition(size.x / 2.0f, RADIUS);
                item->shape.setFillColor(lvl & 1 ? m_red : m_gray);

                item->text.setPosition((int)item->shape.getPosition().x, (int)item->shape.getPosition().y);

            } else {

                //std::cout << " " << item->key << " ";

                float width = (((1 << maxlvl) * (RADIUS*2) + ((1 << (maxlvl-1)) * WIDTH)));

                float step = width / (1 << lvl);
                float pos = m_window->getSize().x/2 - width/2 + step/2;

                //std::cout << pos << std::endl;

                item->shape.setPosition(pos + step * shift, RADIUS + lvl * RADIUS + lvl*HEIGHT);

                item->text.setPosition((int)item->shape.getPosition().x, (int)item->shape.getPosition().y);

                float len = std::sqrt(  std::pow((item->shape.getPosition().x - item->root->shape.getPosition().x), 2) +
                                        std::pow((item->shape.getPosition().y - item->root->shape.getPosition().y), 2));

                sf::Vector2 pos0 = item->shape.getPosition() + (item->root->shape.getPosition() - item->shape.getPosition()) * (RADIUS / len);
                sf::Vector2 pos1 = item->root->shape.getPosition() + (item->shape.getPosition() - item->root->shape.getPosition()) * (RADIUS / len);

                item->line[0].position = pos0;
                item->line[1].position = pos1;
            }
        } //else { std::cout << " - "; }
        shift++;
    }

    m_items.erase(std::remove(m_items.begin(), m_items.end(), nullptr), m_items.end());
}

//---------------------------

void TreeRenderer::handleEvent(const sf::Event& event) {

    if(event.type == sf::Event::MouseButtonPressed) {
        if(event.mouseButton.button == sf::Mouse::Left) {
            m_lastPosMouse = { (float)event.mouseButton.x, (float)event.mouseButton.y };
            m_lastPosMouse += m_view.getCenter();
            m_pressState = true;
        }
    }

    else if(event.type == sf::Event::MouseButtonReleased) {
        if(event.mouseButton.button == sf::Mouse::Left) {
            m_pressState = false;

            if(m_focusItem != nullptr)
                m_focusItem->unfocused();

            sf::Vector2f pos = { (float)event.mouseButton.x, (float)event.mouseButton.y };
            pos += (m_window->getView().getCenter() - m_window->getView().getSize() / 2.0f);

            m_focusItem = this->testPoint(pos);

            if(m_focusItem != nullptr)
                m_focusItem->focused();
        }
    }

    else if(event.type == sf::Event::MouseMoved) {
        if(m_pressState) {
            m_currentPosMouse = { (float)event.mouseMove.x, (float)event.mouseMove.y };
            sf::Vector2f pos = m_lastPosMouse - m_currentPosMouse;

            m_view.setCenter(pos);
            m_window->setView(m_view);
        }
    }

    else if(event.type == sf::Event::LostFocus)
        m_pressState = false;
}

//---------------------------

void TreeRenderer::update(sf::Time elapsedTime) {

    m_time += elapsedTime;

    if(m_focusItem)
        m_focusItem->shape.setOutlineColor({ 0, 170, (sf::Uint8)(std::abs(std::sin(m_time.asSeconds())) * 255) });
}

//---------------------------

void TreeRenderer::clear() {

    if(m_focusItem) {
        m_focusItem->unfocused();
        m_pressState = false;
        m_focusItem = nullptr;
    }

    for(Item* item : m_items)
        delete item;

    m_items.clear();
    m_items.shrink_to_fit();
}

//---------------------------
