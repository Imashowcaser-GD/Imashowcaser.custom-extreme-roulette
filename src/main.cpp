#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace geode::prelude;

std::vector<std::string> separarIDs(const std::string& textoCompleto) {
    std::vector<std::string> resultado;
    std::stringstream ss(textoCompleto);
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t primero = item.find_first_not_of(" ");
        size_t ultimo = item.find_last_not_of(" ");
        if (primero != std::string::npos && ultimo != std::string::npos) {
            resultado.push_back(item.substr(primero, (ultimo - primero + 1)));
        }
    }
    return resultado;
}

// Ventana personalizada para la ruleta animada
class RoulettePopup : public geode::Popup<std::vector<std::string> const&> {
protected:
    std::vector<std::string> m_ids;
    CCSprite* m_thumbnailSprite = nullptr;
    CCLabelBMFont::create* m_titleLabel = nullptr;
    int m_girosRestantes = 0;
    float m_delayActual = 0.05f; // Velocidad inicial del cambio de imágenes

    bool setup(std::vector<std::string> const& ids) override {
        m_ids = ids;
        this->setTitle("Extreme Custom Roulette");

        // Crear un sprite vacío inicial para la miniatura
        m_thumbnailSprite = CCSprite::create();
        m_thumbnailSprite->setPosition({m_size.width / 2, m_size.height / 2 + 10});
        m_mainLayer->addChild(m_thumbnailSprite);

        // Iniciar la animación: cambia de imagen 25 veces antes de frenar
        m_girosRestantes = 25;
        std::srand(std::time(0));
        
        this->cambiarImagenAleatoria(0.0f);

        return true;
    }

    void cambiarImagenAleatoria(float dt) {
        if (m_girosRestantes <= 0) {
            // La ruleta se detuvo, el nivel actual en pantalla es el ganador
            auto alert = FLAlertLayer::create("Resultado", "¡Tu desafio esta listo!", "A JUGAR");
            alert->show();
            return;
        }

        // Elegir un ID al azar para la animación
        int idx = std::rand() % m_ids.size();
        std::string idActual = m_ids[idx];

        // Buscar la miniatura
        auto rutaImagen = Mod::get()->getSavedVariablesDir() / ".." / "irwansha.level_thumbnails" / (idActual + ".png");

        if (std::filesystem::exists(rutaImagen)) {
            // Cambiar la textura del sprite existente para que parpadee con el nuevo nivel
            m_thumbnailSprite->setTexture(CCTextureCache::sharedTextureCache()->addImage(rutaImagen.string().c_str()));
            m_thumbnailSprite->setScale(0.6f);
            m_thumbnailSprite->setVisible(true);
        } else {
            m_thumbnailSprite->setVisible(false); // Ocultar si no hay miniatura descargada
        }

        m_girosRestantes--;
        
        // Multiplicamos el delay para que vaya más lento en cada cuadro (Efecto fricción)
        m_delayActual *= 1.12f; 

        // Programar el siguiente cambio de imagen con el nuevo retraso largo
        this->scheduleOnce(schedule_selector(RoulettePopup::cambiarImagenAleatoria), m_delayActual);
    }

public:
    static RoulettePopup* create(std::vector<std::string> const& ids) {
        auto ret = new RoulettePopup();
        if (ret && ret->initAnchored(240.f, 160.f, ids, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// Hook al menú principal para abrir nuestra ruleta
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto rightMenu = this->getChildByID("right-side-menu");
        if (rightMenu) {
            auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            auto myButton = CCMenuItemSpriteExtra::create(
                btnSprite,
                this,
                menu_selector(MyMenuLayer::onRouletteClick)
            );
            rightMenu->addChild(myButton);
            rightMenu->updateLayout();
        }
        return true;
    }

    void onRouletteClick(CCObject* sender) {
        std::string textoAjustes = Mod::get()->getSettingValue<std::string>("lista-niveles");
        std::vector<std::string> misIDs = separarIDs(textoAjustes);

        if (misIDs.empty()) {
            FLAlertLayer::create("Error", "La lista de IDs esta vacia.", "OK")->show();
            return;
        }

        // Abrir la ventana emergente de la ruleta animada
        RoulettePopup::create(misIDs)->show();
    }
};

