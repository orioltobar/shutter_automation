package controls

import BaseScreenModel
import cafe.adriel.voyager.core.model.screenModelScope
import data.ShuttersApi
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.launch
import model.NetworkException
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.stringResource
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.error_no_network
import shutters.composeapp.generated.resources.error_unknown


@OptIn(ExperimentalResourceApi::class)
class ControlsScreenModel(private val repository: ShuttersApi)
        : BaseScreenModel<ControlsVM, ControlsAction>() {
    override val _state: MutableStateFlow<ControlsVM> = MutableStateFlow(ControlsVM.empty())

    override infix fun reduce(action: ControlsAction) {
        super.reduce(action)
        when (action) {
            is LowerButtonClicked -> onLowerButtonClicked()
            is RiseButtonClicked -> onRiseButtonClicked()
            ErrorMessageShown -> onErrorMessageShown()
        }
    }

    private fun onLowerButtonClicked() {
        screenModelScope.launch {
            repository.triggerDown()
                .onFailure(::onFailure)
        }
    }

    private fun onFailure(throwable: Throwable) {
        currentState = currentState.copy(errorMessage = if (throwable is NetworkException) {
            Res.string.error_no_network
        } else {
            Res.string.error_unknown
        })
    }

    private fun onRiseButtonClicked() {
        screenModelScope.launch {
            repository.triggerUp()
                .onFailure(::onFailure)
        }
    }

    private fun onErrorMessageShown() {
        currentState = currentState.copy(errorMessage = null)
    }
}