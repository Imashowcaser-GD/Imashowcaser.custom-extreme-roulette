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

    // 🔥 LA FUNCIÓN DEL SHUFFLE
    void mezclarRuletaActiva() {
        if (nivelesActuales.empty()) {
            FLAlertLayer::create("Error", "¡No puedes mezclar una ruleta vacía!", "OK")->show();
            return;
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(nivelesActuales.begin(), nivelesActuales.end(), g);

        FLAlertLayer::create("¡Mezclado!", "El orden de los niveles ha sido cambiado al azar.", "A jugar")->show();
    }
};

// ==========================================
// 2. LA CAPA VISUAL (Popup del Gestor de Ranuras)
// ==========================================
class RouletteMenuLayer : public Geode::Popup<> {
protected:
    bool setup() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Ocultamos el fondo gris genérico de Geode para que luzca tu tablero limpio
        this->m_bgSprite->setVisible(false);
        
        // 1. CAMBIA AQUÍ si tu archivo de tablero se llamaba diferente (Captura 1000018486)
        auto fondoCuadricula = CCSprite::createWithSpriteFrameName("1000018486.png");
        if (fondoCuadricula) {
            fondoCuadricula->setPosition(winSize / 2);
            fondoCuadricula->setScale(0.85f); // Ajuste de tamaño para que quepa en la pantalla
            m_mainLayer->addChild(fondoCuadricula);
        } else {
            // Plan B de emergencia por si el nombre no coincide, para que no crashee
            auto bgFallback = CCScale9Sprite::create("GJ_square02.png");
            bgFallback->setContentSize({320, 240});
            bgFallback->setPosition(winSize / 2);
            m_mainLayer->addChild(bgFallback);
        }

        // Coordenadas calibradas para centrar las miniaturas en cada uno de tus 4 bloques marrones
        CCPoint posicionesSlots[4] = {
            ccp(winSize.width / 2 - 64, winSize.height / 2 + 60), // Slot 1 (Superior Izquierda)
            ccp(winSize.width / 2 + 64, winSize.height / 2 + 60), // Slot 2 (Superior Derecha)
            ccp(winSize.width / 2 - 64, winSize.height / 2 - 60), // Slot 3 (Inferior Izquierda)
            ccp(winSize.width / 2 + 64, winSize.height / 2 - 60)  // Slot 4 (Inferior Derecha)
        };

        // Renderizado automático de miniaturas dentro de la cuadrícula
        for (int i = 0; i < 4; i++) {
            int numeroSlot = i + 1;
            CCPoint centroSlot = posicionesSlots[i];

            if (ExtremeRouletteManager::get()->existeSlot(numeroSlot)) {
                
                // Texto indicador del número de ruleta cargada
                auto labelTitulo = CCLabelBMFont::create((std::string("SLOT ") + std::to_string(numeroSlot)).c_str(), "goldFont.fnt");
                labelTitulo->setScale(0.35f);
                labelTitulo->setPosition(ccp(centroSlot.x, centroSlot.y + 40));
                m_mainLayer->addChild(labelTitulo);

                // 2. CAMBIA AQUÍ si tu ruleta con marco verde se llamaba diferente (Captura 1000018487)
                auto ruletaMini = CCSprite::createWithSpriteFrameName("1000018487.png");
                if (ruletaMini) {
                    ruletaMini->setScale(0.4f); // Escalado pequeño para encajar justo en el bloque marrón
                    ruletaMini->setPosition(centroSlot);
                    m_mainLayer->addChild(ruletaMini);
                }
            } else {
                // Si el slot está vacío, mostramos un texto sutil
                auto labelVacio = CCLabelBMFont::create("VACIO", "bigFont.fnt");
                labelVacio->setScale(0.3f);
                labelVacio->setOpacity(90);
                labelVacio->setPosition(centroSlot);
                m_mainLayer->addChild(labelVacio);
            }
        }

        return true;
    }

public:
    static RouletteMenuLayer* create() {
        auto ret = new RouletteMenuLayer();
        // Inicializa el Popup con el tamaño ideal para contener tu asset
        if (ret && ret->initAnchored(340, 260)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// ==========================================
// 3. HOOK EN EL MENÚ PRINCIPAL (Botón inferior izquierdo)
// ==========================================
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // Metemos el botón directamente al menú de abajo original del juego
        auto bottomMenu = this->getChildByID("bottom-menu");
        if (bottomMenu) {
            
            // 3. CAMBIA AQUÍ si tu botón de Shuffle o el ícono que elegiste se llamaba diferente
            // Usamos provisionalmente el de Shuffle (1000018485) como botón de acceso si así lo deseas
            auto rouletteIcon = CCSprite::createWithSpriteFrameName("1000018485.png");
            
            // Si el sprite no se encuentra por problemas de nombre, carga un botón nativo para evitar crasheos
            if (!rouletteIcon) {
                rouletteIcon = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
            }

            auto rouletteBtn = CCMenuItemSpriteExtra::create(
                rouletteIcon,
                this,
                menu_selector(MyMenuLayer::onRouletteClick)
            );

            // Añadir al menú y reordenar el espacio de los botones automáticamente
            bottomMenu->addChild(rouletteBtn);
            bottomMenu->updateLayout();
        }

        return true;
    }

    void onRouletteClick(CCObject* sender) {
        RouletteMenuLayer::create()->show();
    }
};
