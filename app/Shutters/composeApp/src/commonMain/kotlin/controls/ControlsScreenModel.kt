package controls

import BaseScreenModel
import cafe.adriel.voyager.core.model.screenModelScope
import data.ShuttersApi
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.launch
import model.BlindType
import model.NetworkException
import org.jetbrains.compose.resources.ExperimentalResourceApi
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.error_no_network
import shutters.composeapp.generated.resources.error_unknown


@OptIn(ExperimentalResourceApi::class)
class ControlsScreenModel(private val repository: ShuttersApi) :
    BaseScreenModel<ControlsVM, ControlsAction>() {
    override val _state: MutableStateFlow<ControlsVM> = MutableStateFlow(ControlsVM.empty())

    override infix fun reduce(action: ControlsAction) {
        super.reduce(action)
        when (action) {
            is BlindSelectorClicked -> onBlindSelectorClicked(type = action.type)
            is LowerButtonClicked -> onLowerButtonClicked()
            is RiseButtonClicked -> onRiseButtonClicked()
            is StopButtonClicked -> onStopButtonClicked()
            is ErrorMessageShown -> onErrorMessageShown()
        }
    }

    private fun onBlindSelectorClicked(type: BlindType) {
        screenModelScope.launch {
            val currentState = _state.value
            _state.emit(currentState.copy(selectedBlind = type))
        }
    }

    private fun onStopButtonClicked() {
        screenModelScope.launch {
            repository.stop(currentState.selectedBlind)
                .onFailure(::onFailure)
        }
    }

    private fun onLowerButtonClicked() {
        screenModelScope.launch {
            repository.triggerDown(currentState.selectedBlind)
                .onFailure(::onFailure)
        }
    }

    private fun onFailure(throwable: Throwable) {
        currentState = currentState.copy(
            errorMessage = if (throwable is NetworkException) {
                Res.string.error_no_network
            } else {
                Res.string.error_unknown
            }
        )
    }

    private fun onRiseButtonClicked() {
        screenModelScope.launch {
            repository.triggerUp(currentState.selectedBlind)
                .onFailure(::onFailure)
        }
    }

    private fun onErrorMessageShown() {
        currentState = currentState.copy(errorMessage = null)
    }
}