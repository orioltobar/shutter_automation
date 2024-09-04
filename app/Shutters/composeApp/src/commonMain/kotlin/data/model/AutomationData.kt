package data.model

import kotlinx.serialization.*

@Serializable
class AutomationData(
    val time: String,
    @SerialName("is_active")
    val isActive: Boolean,
    @SerialName("is_triggered")
    val isTriggered: Boolean? = false
)