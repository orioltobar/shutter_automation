package model


data class Status(
    val day: Int,
    val time: String,
    val automations: List<Automation>
)