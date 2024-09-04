package settings

data class SettingsVM(
    val isApplyButtonEnabled: Boolean,
    val host: String,
    val port: Int
) {
    companion object {
        fun empty() =
            SettingsVM(
                false,
                host = "",
                port = 0)
    }
}

sealed interface SettingsAction
data object ViewCreated: SettingsAction
data object ApplyClicked: SettingsAction
data class HostChanged(val host: String): SettingsAction
data class PortChanged(val port: String): SettingsAction
