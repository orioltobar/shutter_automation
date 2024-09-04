package data.model

import kotlinx.serialization.Serializable

@Serializable
data class ResponseWrapper<T>(
    val data: T,
    val status: String
)

enum class ResponseStatus(val value: String) {
    Success("success"),
    Fail("fail")
}