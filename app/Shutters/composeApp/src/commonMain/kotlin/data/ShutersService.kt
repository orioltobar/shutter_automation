package data

import model.Automation
import model.Status

interface ShuttersApi {
    fun setHost(host: String): Unit

    fun getHost(): String

    fun getPort(): Int

    fun setPort(port: Int): Unit

    suspend fun getState(): Result<Status>

    suspend fun triggerDown(): Result<Unit>

    suspend fun triggerUp(): Result<Unit>

    suspend fun setAutomationActive(automation: Automation): Result<Automation>

    suspend fun setAutomationTime(automation: Automation): Result<Automation>

    suspend fun setAlarmDown(time: String? = null, isActive: Boolean? = null): Result<Automation>

    suspend fun setAlarmUp(time: String? = null, isActive: Boolean? = null): Result<Automation>
}
