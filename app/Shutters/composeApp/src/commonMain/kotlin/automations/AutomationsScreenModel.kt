package automations

import BaseScreenModel
import cafe.adriel.voyager.core.model.screenModelScope
import data.ShuttersApi
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.launch
import kotlinx.datetime.LocalTime
import model.Automation
import model.Exceptions
import model.NetworkException
import org.jetbrains.compose.resources.ExperimentalResourceApi
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.error_generic
import shutters.composeapp.generated.resources.error_unknown

@OptIn(ExperimentalResourceApi::class)
class AutomationsScreenModel(private val repository: ShuttersApi): BaseScreenModel<AutomationsVM, AutomationsAction>() {
    override val _state: MutableStateFlow<AutomationsVM> = MutableStateFlow(AutomationsVM.empty())


    override infix fun reduce(action: AutomationsAction) {
        super.reduce(action)

        when(action) {
            is ViewCreated -> loadData()
            is AutomationTimeClicked -> onTimeClicked(action.index)
            is TimeSet -> onTimeSet(action.time)
            TimeSetDismissed -> onTimeSetDismissed()
            is AutomationActiveToggle -> setAutomationEnabled(action.index)
            RetryButtonClicked -> onRetryButtonClicked()
            ErrorMessageShown -> onErrorMessageShown()
        }
    }

    private fun loadData() {
        screenModelScope.launch {
            currentState = currentState.copy(isLoading = true)
            repository.getState()
                .onSuccess {
                    currentState = currentState.copy(
                        isLoading = false,
                        automations = it.automations.map { automation -> automation.toVM() },
                        time = it.time,
                        day = it.day
                    )
                }.onFailure {
                    currentState = if (it is NetworkException) {
                        currentState.copy(isLoading = false, showNetworkError = true)
                    } else if (it is Exceptions){
                        currentState.copy(isLoading = false,
                            errorMessage = ErrorMessage(
                                Res.string.error_generic,
                                arrayOf(it::class.simpleName.orEmpty(), it.exception.message.orEmpty())))
                    } else {
                        currentState.copy(isLoading = false,
                            errorMessage = ErrorMessage(Res.string.error_unknown))
                    }
                }
        }
    }

    private fun setAutomationEnabled(index: Int) {
        screenModelScope.launch {
            currentState = currentState.copy(isLoading = true)
            val automation = currentState.automations[index]
            repository.setAutomationActive(automation.copy(isActive = !automation.isActive).toModel())
                .onSuccess {
                    currentState = nextState(index, it)
                }.onFailure { currentState = currentState.copy(isLoading = false) }
        }
    }

    private fun nextState(index: Int, automationData: Automation) =
        currentState.copy(
            isLoading = false,
            automations = currentState.automations.update(index, automationData.toVM()))

    private fun List<AutomationVM>.update(index: Int, item: AutomationVM): List<AutomationVM> =
        slice(0 until index) + item + slice(index+1 until size)

    private fun onTimeClicked(index: Int) {
        currentState = currentState.copy(
            automations = currentState.automations.update(
                index,
                currentState.automations[index].copy(isSetting = true))
        )
    }

    private fun onTimeSet(time: LocalTime) {
        val index = currentState.automations.indexOfFirst { it.isSetting }
        val automation = currentState.automations[index].copy(isSetting = false)
        screenModelScope.launch {
            currentState = currentState.copy(
                isLoading = true,
                automations = currentState.automations.update(index, automation))
            repository.setAutomationTime(automation.copy(hour = time.hour, minute = time.minute).toModel())
                .onSuccess {
                    currentState = currentState.copy(
                        isLoading = false,
                        automations = currentState.automations.update(index, it.toVM()))
                }.onFailure { currentState = currentState.copy(isLoading = false) }
        }
    }

    private fun onTimeSetDismissed() {
        val automation = currentState.automations.withIndex().first { it.value.isSetting }
        currentState = currentState.copy(
            automations = currentState.automations.update(
                automation.index,
                automation.value.copy(isSetting = false))
        )
    }

    private fun onRetryButtonClicked() {
        currentState = currentState.copy(showNetworkError = false)
        loadData()
    }

    private fun onErrorMessageShown() {
        currentState = currentState.copy(errorMessage = null)
    }
}