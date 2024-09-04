package settings

import BaseScreenModel
import data.ShuttersApi
import kotlinx.coroutines.flow.MutableStateFlow

class SettingsScreenModel(private val repository: ShuttersApi)
    : BaseScreenModel<SettingsVM, SettingsAction>() {
    override val _state: MutableStateFlow<SettingsVM> = MutableStateFlow(SettingsVM.empty())

    override fun reduce(action: SettingsAction) {
        super.reduce(action)

        when(action) {
            is ApplyClicked -> onApplyClicked()
            ViewCreated -> loadData()
            is HostChanged -> onHostChanged(action.host)
            is PortChanged -> onPortChanged(action.port)
        }
    }

    private fun loadData() {
        currentState = currentState.copy(
            host = repository.getHost(),
            port = repository.getPort()
        )
    }

    private fun onApplyClicked() {
        repository.setHost(currentState.host)
        repository.setPort(currentState.port)
    }

    private fun onPortChanged(port: String) {
        port.toIntOrNull()?.let{
            currentState = currentState.copy(port = it)
                .enableApplyButtonIfNeeded()
        }
    }

    private fun SettingsVM.enableApplyButtonIfNeeded() =
        copy(isApplyButtonEnabled = host.isNotBlank() && port > 0)

    private fun onHostChanged(host: String) {
        currentState = currentState.copy(host = host).enableApplyButtonIfNeeded()
    }
}