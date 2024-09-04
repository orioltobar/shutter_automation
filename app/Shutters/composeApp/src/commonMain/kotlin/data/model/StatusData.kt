package data.model

import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable

@Serializable
data class StatusData(
    val day: Int,
    @SerialName("current_time")
    val time: String,
    @SerialName("alarm_up")
    val alarmUp: AutomationData,
    @SerialName("alarm_down")
    val alarmDown: AutomationData
)
