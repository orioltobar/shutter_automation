package controls

import model.BlindType
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.StringResource

@OptIn(ExperimentalResourceApi::class)
data class ControlsVM(
    val isLoading: Boolean,
    val errorMessage: StringResource?,
    val selectedBlind: BlindType = BlindType.ALL,
) {
    companion object {
        fun empty() = ControlsVM(
            isLoading = false,
            errorMessage = null
        )
    }
}

sealed interface ControlsAction
data object RiseButtonClicked : ControlsAction
data object LowerButtonClicked : ControlsAction
data object StopButtonClicked : ControlsAction
data object ErrorMessageShown : ControlsAction
data class BlindSelectorClicked(val type: BlindType) : ControlsAction
