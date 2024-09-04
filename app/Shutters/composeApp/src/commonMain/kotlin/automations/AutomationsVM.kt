@file:OptIn(ExperimentalResourceApi::class)

package automations

import kotlinx.datetime.LocalTime
import model.Automation
import model.AutomationType
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.StringResource


@OptIn(ExperimentalResourceApi::class)
data class AutomationsVM(
    val isLoading: Boolean,
    val time: String,
    val day: Int,
    val showNetworkError: Boolean,
    val errorMessage: ErrorMessage?,
    val automations: List<AutomationVM>,
) {
    companion object {
        fun empty() =
            AutomationsVM(
                isLoading = false,
                time = "",
                day = 0,
                errorMessage = null,
                showNetworkError = false,
                automations = listOf(),
            )
    }
}

class ErrorMessage(
    val message: StringResource,
    val args: Array<String> = arrayOf()
)

data class AutomationVM(
    val id: Int,
    val type: AutomationType,
    val hour: Int,
    val minute: Int,
    val isActive: Boolean,
    val isTriggered: Boolean,
    val isSetting: Boolean
) {
    companion object {
        fun empty() =
            AutomationVM(
                id = 0,
                type = AutomationType.Rise,
                hour = 0,
                minute = 0,
                isActive = false,
                isTriggered = false,
                isSetting = false
            )
    }
}

fun Automation.toVM() =
    AutomationVM(
        id = id,
        type = type,
        hour = time.split(":").first().toInt(),
        minute = time.split(":").last().toInt(),
        isActive =  isActive,
        isSetting = false,
        isTriggered = isTriggered ?: false
    )

fun AutomationVM.toModel() =
    Automation(
        id = id,
        type = type,
        time = "${hour}:${minute}",
        isActive = isActive,
        isTriggered = isTriggered
    )

sealed interface AutomationsAction
data object ViewCreated: AutomationsAction
data class AutomationActiveToggle(val index: Int): AutomationsAction
data class AutomationTimeClicked(val index: Int): AutomationsAction
data object TimeSetDismissed: AutomationsAction
data object RetryButtonClicked: AutomationsAction
data object ErrorMessageShown: AutomationsAction
data class TimeSet(val time: LocalTime): AutomationsAction
