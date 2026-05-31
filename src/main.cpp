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

        // Coordenadas calibradas con la sintaxis moderna de Cocos2d-x (Se quitó ccp)
        CCPoint posicionesSlots[4] = {
            CCPoint(winSize.width / 2 - 64, winSize.height / 2 + 60), // Slot 1 (Superior Izquierda)
            CCPoint(winSize.width / 2 + 64, winSize.height / 2 + 60), // Slot 2 (Superior Derecha)
            CCPoint(winSize.width / 2 - 64, winSize.height / 2 - 60), // Slot 3 (Inferior Izquierda)
            CCPoint(winSize.width / 2 + 64, winSize.height / 2 - 60)  // Slot 4 (Inferior Derecha)
        };

        // Renderizado automático de miniaturas dentro de la cuadrícula
        for (int i = 0; i < 4; i++) {
            int numeroSlot = i + 1;
            CCPoint centroSlot = posicionesSlots[i];

            if (ExtremeRouletteManager::get()->existeSlot(numeroSlot)) {
                
                // Texto indicador del número de ruleta cargada
                auto labelTitulo = CCLabelBMFont
