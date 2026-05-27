#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <cocos2d.h>
#include <fstream>
#include <string>
#include <vector>

using namespace geode::prelude;

// Creamos una clase para manejar la lógica de nuestra ruleta
class ExtremeRouletteManager {
public:
    std::vector<std::string> nivelesImportados;

    static ExtremeRouletteManager* get() {
        static ExtremeRouletteManager instance;
        return &instance;
    }

    // Función para leer el archivo .wheel desde la carpeta del mod
    void cargarArchivoWheel() {
        nivelesImportados.clear();

        // Ruta en Android: geode/mods/lmashowcaser.custom-extreme-roulette/ruleta.wheel
        auto carpetaMod = Mod::get()->getConfigDir();
        auto rutaArchivo = carpetaMod / "ruleta.wheel";

        std::ifstream archivo(rutaArchivo);
        if (archivo.is_open()) {
            std::string linea;
            while (std::getline(archivo, linea)) {
                // Si no está vacía, añadimos la línea (nombre del nivel) al vector
                if (!linea.empty()) {
                    nivelesImportados.push_back(linea);
                }
            }
            archivo.close();
            
            // Avisamos al usuario que se cargó con éxito
            std::string msg = "Se cargaron " + std::to_string(nivelesImportados.size()) + " niveles.";
            FLAlertLayer::create("¡Éxito!", msg.c_str(), "Genial")->show();
        } else {
            // Si no encuentra el archivo, podemos cargar una lista por defecto o avisar
            FLAlertLayer::create("Aviso", "No se encontró 'ruleta.wheel' en la carpeta config del mod.", "OK")->show();
        }
    }

    // Función que abre la ruleta e inicia el tutorial si es la primera vez
    void abrirMenuRuleta() {
        // Comprobamos el "interruptor" guardado en Geode
        bool yaVioTutorial = Mod::get()->getSavedValue<bool>("tutorial-completado", false);

        if (!yaVioTutorial) {
            // Mostramos el tutorial de bienvenida
            FLAlertLayer::create(
                "Tutorial de la Ruleta", 
                "¡Bienvenido! Pon tu archivo <cl>.wheel</c> dentro de la carpeta del mod en Geode.\nCada línea del archivo debe ser un Extreme Demon diferente.", 
                "Entendido"
            )->show();

            // Guardamos el valor para que no vuelva a molestar
            Mod::get()->setSavedValue<bool>("tutorial-completado", true);
        } else {
            // Si ya vio el tutorial, intentamos cargar el archivo y abrir la ruleta directamente
            cargarArchivoWheel();
            
            // Aquí irá más adelante la lógica para hacer girar el CCSprite de tu ruleta
        }
    }
};

// Inyectamos el botón en el menú principal de Geometry Dash
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // Obtenemos el tamaño de la pantalla del dispositivo
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Creamos el botón usando el icono que vas a meter en tu modsheet.png
        auto rouletteIcon = CCSprite::createWithSpriteFrameName("tu_icono_ruleta.png");
        
        // Si no tienes el modsheet listo aún, puedes usar un botón del juego temporalmente para probar:
        // auto rouletteIcon = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");

        auto rouletteBtn = CCMenuItemSpriteExtra::create(
            rouletteIcon,
            this,
            menu_selector(MyMenuLayer::onRouletteClick)
        );

        // Creamos el contenedor del menú para el botón
        auto menu = CCMenu::create(rouletteBtn, nullptr);

        // Lo alineamos exactamente en la esquina inferior izquierda (X: 25, Y: 25 para darle margen)
        menu->setPosition({ 25.0f, 25.0f });

        // Añadimos el menú a la capa principal
        this->addChild(menu);

        return true;
    }

    // Esta función se ejecuta al tocar el botón de la esquina
    void onRouletteClick(CCObject* sender) {
        ExtremeRouletteManager::get()->abrirMenuRuleta();
    }
};
