package data

import com.russhwolf.settings.Settings
import com.russhwolf.settings.get
import com.russhwolf.settings.set

class LocalSettings {
    private val settings = Settings()
    var host: String = settings.get<String>(HOST_KEY) ?: DEFAULT_HOST
        set(value) {
            settings[HOST_KEY] = value
            field = value
        }
    var port: Int = settings.get<Int>(PORT_KEY) ?: DEFAULT_PORT
        set(value) {
            settings[PORT_KEY] = value
            field = value
        }

    companion object {
        const val DEFAULT_PORT = 80
        const val DEFAULT_HOST = "localhost"
        const val HOST_KEY = "host_key"
        const val PORT_KEY = "port_key"
    }
}