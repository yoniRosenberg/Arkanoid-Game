#include "tiles.hpp"
#include <SFML/Graphics.hpp>

namespace arkanoid{


Tiles::Tiles(sf::Vector2f a_position, sf::Vector2f a_size, int a_rows_num, int a_cols_num, int a_level_num)
: m_colors{{sf::Color::Red, 90}, {sf::Color::Green, 80}, {sf::Color::Blue, 100}, {sf::Color::Cyan, 70} 
, {sf::Color::White, 50}, {sf::Color::Magenta, 110}, {sf::Color::Yellow, 0}}
, m_rows_num(a_rows_num)
, m_col_num(a_cols_num)
, m_tile_width(100.0f)
, m_tile_height(40.0f)
, m_position(a_position)
, m_level_num(a_level_num)
, m_size(a_size)
,m_blow_flag(false)
{
    create_tiles();
}


Tiles::Tiles(const Tiles& a_other)
: m_tiles(a_other.m_tiles)
,m_colors(a_other.m_colors)
,m_rows_num(a_other.m_rows_num)
,m_col_num(a_other.m_col_num)
,m_tile_width(a_other.m_tile_width)
,m_tile_height(a_other.m_tile_height)
,m_position(a_other.m_position)
,m_level_num(a_other.m_level_num)
{
    // create_tiles();
}


Tiles::~Tiles()
{
    m_tiles.clear();
}
    



void Tiles::draw_tiles(sf::RenderWindow& a_window)
{
    for ( const auto& tile : m_tiles){
       
        tile->draw_rectangle(a_window);
    }
    // static sf::Clock Clock; 
    // for (auto it = m_explosions.begin(); it != m_explosions.end();) {
    //     a_window.draw(*it);

    // }
}
    



void Tiles::create_tiles()
{
    if(m_level_num == 1){
        first_level();  
    }
    else if(m_level_num == 2){
        second_level();
    }else{
        third_level();
    }
}


size_t Tiles::num_of_tiles()
{
    return m_tiles.size();
}


void Tiles::clear_all_tiles() 
{
    m_tiles.clear();
}

int Tiles::remove_tile(std::list<std::shared_ptr<RectangleObject>>::iterator it)
{
    int points = 0;

    if (*it != nullptr){   
        if (typeid(**it) == typeid(LongLivedBrick)){
            points = handle_old_brick(it);
        }else if(typeid(**it) == typeid(BlowBrick)){
            handle_blow_brick(it);
            points = 150;       
        }else{   
            sf::Color color = (*it)->color();
            if (color != sf::Color::Yellow){            
                for (const auto& colorInfo : m_colors){                
                    if (colorInfo.color == color){               
                        points = colorInfo.points;
                        break;
                    }
                }
                m_tiles.erase(it);
            }
        }
    }
    return points;
}


void Tiles::first_level()
{
    for (int row = 0; row < m_rows_num; ++row){
    
        const ColorInfo& colorInfo = m_colors[row % m_colors.size()];
        sf::Color color = colorInfo.color;

        for (int col = 0; col < m_col_num; ++col){
        
            sf::Vector2f tile_position(m_position.x + col * m_tile_width, m_position.y + row * m_tile_height);           
            m_tiles.push_back(std::make_shared<RectangleObject>(tile_position, sf::Vector2f(m_tile_width, m_tile_height), color));
        }
    }
}


void Tiles::second_level()
{
    for (int col = 0; col < m_col_num; ++col){   
        const ColorInfo& colorInfo = m_colors[col % m_colors.size()];
        sf::Color color = colorInfo.color;

        for (int row = m_rows_num - col; row > 0; --row){
            sf::Vector2f tile_position(m_position.x + col * m_tile_width, m_position.y - row * m_tile_height);
            if (row  == 1 && col != m_col_num -1){            
                m_tiles.push_back(std::make_shared<LongLivedBrick>(tile_position, sf::Vector2f(m_tile_width, m_tile_height), sf::Color::White));
            } else {    
                m_tiles.push_back(std::make_shared<RectangleObject>(tile_position, sf::Vector2f(m_tile_width, m_tile_height), color));
            }
        }
    }
}


void Tiles::third_level()
{
    for (int row = 1; row < m_rows_num +1; ++row){
        if (row % 2 == 0) {
                continue;
        }
        const ColorInfo& colorInfo = m_colors[row % m_colors.size()];
        sf::Color color;

        for (int col = 0; col < m_col_num; ++col){
        
            if((col < 2 && row == 3) ||(col > 3 && row == 7)){
                color = sf::Color::White;
            }else{
                color = colorInfo.color;
            }
            sf::Vector2f tile_position(m_position.x + col * m_tile_width, m_position.y + row * m_tile_height);
            if((col + row) % 3 == 0){
                m_tiles.push_back(std::make_shared<BlowBrick>(tile_position, sf::Vector2f(m_tile_width, m_tile_height), color));
            }else{    
                m_tiles.push_back(std::make_shared<RectangleObject>(tile_position, sf::Vector2f(m_tile_width, m_tile_height), color));
            }
        }
    }
}

int Tiles::handle_old_brick(std::list<std::shared_ptr<RectangleObject>>::iterator it)
{
    int points = 0;
    (*it)->hit();
    if ((*it)->is_destroyed()){
        (*it)->reset();
        m_tiles.erase(it);
        points = 300;
    }
    return points;
}


void Tiles::handle_blow_brick(std::list<std::shared_ptr<RectangleObject>>::iterator it)
{
    if (*it == nullptr)
    {
        return;
    }
    // m_explosions.emplace_back(it->get()->blow_up());

    std::list<std::shared_ptr<RectangleObject>>::iterator prev_it = std::prev(it);
    std::list<std::shared_ptr<RectangleObject>>::iterator next_it = std::next(it);

    if(*prev_it != nullptr && it != m_tiles.begin()){
        m_tiles.erase(prev_it);
    }
    if(*next_it != nullptr && next_it != m_tiles.end()){
        m_tiles.erase(next_it);
    }
    m_tiles.erase(it);
}



} // namespace arkanoid
