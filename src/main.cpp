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

