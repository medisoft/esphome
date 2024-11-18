import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

esp_now_ns = cg.esphome_ns.namespace('esp_now')
ESPNow = esp_now_ns.class_('ESPNow', cg.Component)


CONF_CHANNEL = "channel"
CONF_PASSWORD = "password"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ESPNow),
    cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=14),  # Canal de Wi-Fi, rango de 1 a 13
    cv.Required(CONF_PASSWORD): cv.All(cv.string, cv.Length(min=2)),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
