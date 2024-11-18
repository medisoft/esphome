import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import component
from esphome.const import CONF_ID

# Definir el namespace del componente
esp_now_ns = cg.esphome_ns.namespace("esp_now")
ESPNow = esp_now_ns.class_("ESPNow", cg.Component)

# Definir las opciones de configuración
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPNow),
}).extend(component.COMPONENT_SCHEMA)

# Función para registrar el componente
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
