#include <SFML/Graphics.hpp>
#include <vector>
#include <exception>
#include <algorithm>

/*
Current pattern 	111 	110 	101 	100 	011 	010 	001 	000
New state for center cell 	0 	1 	1 	0 	1 	1 	1 	0
*/

class Rule110 : sf::NonCopyable
{
public:

    Rule110(unsigned size)
    {
        if(size < 5u)
            throw std::runtime_error("Rule110(x) where x < 5u");

        m_buffer.resize(size, 0);
        m_buffer2.resize(size);
        m_pixels.resize(4 * size);
        m_buffer[size - 1] = 1;
        m_rules[0] = 0;
        m_rules[1] = 1;
        m_rules[2] = 1;
        m_rules[3] = 1;
        m_rules[4] = 0;
        m_rules[5] = 1;
        m_rules[6] = 1;
        m_rules[7] = 0;
    }

    void update()
    {
        const int last = m_buffer.size() - 1;
        for(int i = 1; i < last; ++i)
        {
            const int x = 4 * m_buffer[i - 1] + 2 * m_buffer[i] + m_buffer[i + 1];
            m_buffer2[i] = m_rules[x];

            //m_buffer2[]
        }

        //first and last special cases
        m_buffer2[0] = m_rules[2 * m_buffer[0] + m_buffer[1]];
        m_buffer2[last] = m_rules[2 * m_buffer[last - 1] + m_buffer[last]];;

        ++m_updatecount;
        std::swap(m_buffer, m_buffer2);
    }

    const sf::Uint8 * getPixelLine()
    {
        for(int i = 0; i < m_buffer.size(); ++i)
        {
            const int x = m_buffer[i] ? 0xff : 0x0;
            m_pixels[4 * i + 0] = x;
            m_pixels[4 * i + 1] = x;
            m_pixels[4 * i + 2] = x;
            m_pixels[4 * i + 3] = 0xff;
        }//for

        return m_pixels.data();
    }

    int getUpdateCount() const
    {
        return m_updatecount;
    }

private:
    std::vector<char> m_buffer;
    std::vector<char> m_buffer2;
    std::vector<sf::Uint8> m_pixels;
    char m_rules[8];
    int m_updatecount = 0;

};

const unsigned kSize = 1024u;

int main()
{
    sf::RenderWindow win(sf::VideoMode(1024u, 768u), "Rule 110");
    win.setFramerateLimit(60u);

    sf::Texture tex;
    sf::Image img;
    img.create(kSize, kSize, sf::Color::Black);
    tex.loadFromImage(img);

    Rule110 rule(kSize);
    while(win.isOpen())
    {
        sf::Event eve;
        while(win.pollEvent(eve))
        {
            switch(eve.type)
            {
            case sf::Event::Closed:
                win.close();
                break;
            case sf::Event::Resized:
                win.setView(sf::View(sf::FloatRect(0.f, 0.f, eve.size.width, eve.size.height)));
                break;
            }//switch
        }//while

        if(rule.getUpdateCount() < kSize)
        {
            tex.update(rule.getPixelLine(), kSize, 1u, 0u, rule.getUpdateCount());
            rule.update();
        }//if

        win.clear(sf::Color(0x7f7f7fff));
        win.draw(sf::Sprite(tex));
        win.display();
    }//while
}
