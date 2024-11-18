import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

esp_now_ns = cg.esphome_ns.namespace('esp_now')
ESPNow = esp_now_ns.class_('ESPNow', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPNow)
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
