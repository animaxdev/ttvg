#include "IntroScreen.h"

#include <cassert>
#include <iostream>

namespace tt
{

namespace
{

void createMenu(TextList& menuItems,
    const std::vector<std::string>& textlist,
    sf::RenderTarget& window, 
    sf::Font& font)
{
    // first add all the items keeping track of the
    // largest one
    for (const auto& text : textlist)
    {
        auto temp = std::make_shared<sf::Text>(text, font);
        menuItems.push_back(temp);
    }

    auto largest = std::max_element(menuItems.begin(), menuItems.end(),
        [](TextPtr a, TextPtr b) 
        { 
            return a->getLocalBounds().width < b->getLocalBounds().width; 
        });

    assert(largest != menuItems.end());

    auto xpos = window.getSize().x - ((*largest)->getLocalBounds().width + 100.f);
    auto ypos = 100.0f;

    for (const auto& item : menuItems)
    {
        item->setPosition(xpos, ypos);
        ypos += item->getLocalBounds().height + 20.0f;
    }
}

void updateMenu(std::uint16_t selection, TextList& menuItems)
{
    std::for_each(menuItems.begin(), menuItems.end(),
        [](TextPtr item) { item->setFillColor(sf::Color::White); });

    if (selection < menuItems.size())
    {
        menuItems.at(selection)->setFillColor(sf::Color::Yellow);
    }
}

}

IntroScreen::IntroScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    
    if (auto temp = _resources.load<sf::Font>("hobo.ttf"); 
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    auto textobj = std::make_shared<sf::Text>("Lord of the Dumpsters", _font);
    textobj->setPosition(690, 10);
    textobj->setFillColor(sf::Color(255, 215, 9));

    auto bgt = _resources.load<sf::Texture>("images/tommy-1.png");
    if (!bgt)
    {
        throw std::runtime_error("tommy-1.png could not be loaded!");
    }

    _bgt = *bgt; // copy!!!
    _bgt.setRepeated(true);

    _sprite = std::make_shared<sf::Sprite>();
    _sprite->setTexture(_bgt);
    
    float xpos = 0;
    float ypos = (_bgt.getSize().y - _window.getSize().y) * -1.0f;
    _sprite->setPosition(xpos, ypos);
    
    _bgsong = _resources.openUniquePtr<sf::Music>("music/intro.wav");
    if (!_bgsong)
    {
        throw std::runtime_error("music/intro.mp3 could not be loaded!");
    }
    _bgsong->setLoop(true);
    _bgsong->play();

    createMenu(_menuItems, { "Play Game", "Shart", "Exit Game" }, _window, _font);
    updateMenu(_selected, _menuItems);
    for (const auto& item : _menuItems)
    {
        addDrawable(item);
    }

    addDrawable(_sprite);
    addDrawable(textobj);
}

void IntroScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Up)
    {
        if (_selected > 0)
        {
            _selected--;
            std::cout << "selectedA: " << _selected << '\n';
            updateMenu(_selected, _menuItems);
        }
    }
    else if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Down)
    {
        if (_selected < _menuItems.size() - 1)
        {
            _selected++;
            std::cout << "selectedB: " << _selected << '\n';
            updateMenu(_selected, _menuItems);
        }
    }
}

void IntroScreen::timestep()
{
    auto[x, y] = _sprite->getPosition();

    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 15)
    {
        y += 5;
        if (y > 0)
        {
            y = (_bgt.getSize().y - _window.getSize().y) * -1.f;
        }

        _sprite->setPosition(x, y);
        _clock.restart();
    }
}

} // namespace tt