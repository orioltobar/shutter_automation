package model

data class Automation(
    val id: Int,
    val type: AutomationType,
    val time: String,
    val isActive: Boolean,
    val isTriggered: Boolean
)

enum class AutomationType(val value: String) {
    Rise("Rise"),
    Lower("Lower")
}