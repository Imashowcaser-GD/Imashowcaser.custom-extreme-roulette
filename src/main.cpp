#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <cocos2d.h>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm> // Para el Shuffle
#include <random>    // Para el Shuffle

using namespace geode::prelude;

// ==========================================
// 1. EL MÁNAGER (Gestiona archivos, slots y mezcla)
// ==========================================
class ExtremeRouletteManager {
public:
    std::vector<std::string> nivelesActuales; // Aquí se guardan los niveles cargados
    int slotActivo = 1;

    static ExtremeRouletteManager* get() {
        static ExtremeRouletteManager instance;
        return &instance;
    }

    std::filesystem::path getRutaSlot(int slot) {
        return Mod::get()->getConfigDir() / ("ruleta" + std::to_string(slot) + ".wheel");
    }

    bool existeSlot(int slot) {
        return std::filesystem::exists(getRutaSlot(slot));
    }

    // 🔥 LA FUNCIÓN DEL SHUFFLE (La pegamos aquí dentro)
    void mezclarRuletaActiva() {
        if (nivelesActuales.empty()) {
            FLAlertLayer::create("Error", "¡No puedes mezclar una ruleta vacía!", "OK")->show();
            return;
        }

        // Mezclamos el vector usando el reloj del celular/PC
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(nivelesActuales.begin(), nivelesActuales.end(), g);

        FLAlertLayer::create("¡Mezclado!", "El orden de los niveles ha sido cambiado al azar.", "A jugar")->show();
    }
};

// ==========================================
// 2. LA CAPA VISUAL (Tus dos imágenes de interfaz)
// ==========================================
class RouletteMenuLayer : public Geode::Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mis Ruletas");
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Colocamos tu cuadrícula base (Imagen 1)
        auto fondoCuadricula = CCSprite::createWithSpriteFrameName("cuadrícula_base.png");
        fondoCuadricula->setPosition(winSize / 2);
        fondoCuadricula->setScale(0.8f);
        m_mainLayer->addChild(fondoCuadricula);

        // Coordenadas para tus 4 bloques marrones
        CCPoint posicionesSlots[4] = {
            ccp(winSize.width / 2 - 70, winSize.height / 2 + 50), // Slot 1
            ccp(winSize.width / 2 + 70, winSize.height / 2 + 50), // Slot 2
            ccp(winSize.width / 2 - 70, winSize.height / 2 - 50), // Slot 3
            ccp(winSize.width / 2 + 70, winSize.height / 2 - 50)  // Slot 4
        };

        // Renderizado dinámico según si existe el archivo o no (Imagen 2)
        for (int i = 0; i < 4; i++) {
            int numeroSlot = i + 1;
            CCPoint centroSlot = posicionesSlots[i];

            if (ExtremeRouletteManager::get()->existeSlot(numeroSlot)) {
                // Si existe la ruleta, le dibujamos todo el pack encima:
                
                // Texto NAME ROULETTE
                auto labelTitulo = CCLabelBMFont::create("NAME ROULETTE", "goldFont.fnt");
                labelTitulo->setScale(0.5f);
                labelTitulo->setPosition(ccp(centroSlot.x, centroSlot.y + 40));
                m_mainLayer->addChild(labelTitulo);

                // El círculo cromático
                auto ruletaSprite = CCSprite::createWithSpriteFrameName("ruleta_circulo.png");
                ruletaSprite->setScale(0.6f);
                ruletaSprite->setPosition(centroSlot);
                m_mainLayer->addChild(ruletaSprite);

                // La flecha verde
                auto flechaSprite = CCSprite::createWithSpriteFrameName("flecha_puntero.png");
                flechaSprite->setScale(0.6f);
                flechaSprite->setPosition(ccp(centroSlot.x + 35, centroSlot.y)); 
                m_mainLayer->addChild(flechaSprite);
            }
        }

        return true;
    }

public:
    static RouletteMenuLayer* create() {
        auto ret = new RouletteMenuLayer();
        if (ret && ret->initAnchored(320, 240, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// ==========================================
// 3. HOOK EN EL MENÚ PRINCIPAL (Botón esquina izquierda)
// ==========================================
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto rouletteIcon = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"); // Temporal
        auto rouletteBtn = CCMenuItemSpriteExtra::create(
            rouletteIcon,
            this,
            menu_selector(MyMenuLayer::onRouletteClick)
        );

        auto menu = CCMenu::create(rouletteBtn, nullptr);
        menu->setPosition({ 25.0f, 25.0f });
        this->addChild(menu);

        return true;
    }

    void onRouletteClick(CCObject* sender) {
        RouletteMenuLayer::create()->show();
    }
};
