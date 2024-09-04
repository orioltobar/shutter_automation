import cafe.adriel.voyager.core.model.ScreenModel
import cafe.adriel.voyager.core.model.screenModelScope
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.SharedFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch

abstract class BaseScreenModel<S, A>: ScreenModel {
    protected val LOG_TAG = this::class.simpleName

    protected abstract val _state: MutableStateFlow<S>
    val state: StateFlow<S> by lazy { _state }

    protected open var currentState: S
        get() = _state.value
        set(nextState) {
            _state.value = nextState
        }

    open infix fun reduce(action: A) {
        println("[$LOG_TAG] ACTION: $action")
    }
}
